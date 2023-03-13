# -*- coding: utf-8 -*-
# Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
"""
Collect system state
"""

import time
import logging
import pandas as pd
from mpctool.ipmi_opt.fan_controller import FanController
from mpctool.ipmi_opt import mpc_controller as mpcc
from mpctool.common import sys_tool as stool
from mpctool.common import mpc_const as CST


STABLE_TH = 0.2


class Queue():
    def __init__(self, max_size):
        self.array = []
        if max_size > 0:
            self.max_size = max_size
        else:
            self.max_size = 1

    def is_full(self):
        return self.max_size <= len(self.array)

    def push(self, value):
        if self.is_full():
            del self.array[0]
        self.array.append(value)

    def is_stable(self):
        if self.is_full():
            if max(self.array) - min(self.array) <= STABLE_TH:
                return True
        return False


def _get_sys_state_from_bmc(state):
    content = mpcc.get_sys_state()
    if content:
        state["PowerConsumedWatts"] = content["PowerConsumedWatts"]
        state["FanTotalPowerWatts"] = content["FanTotalPowerWatts"]
        state["FanSpeedPercent"] = content["FanSpeedPercent"]
        state["InletTemp"] = content["InletTemp"]
        state["cpu_temp"] = content["cpu_temp"]
        state["other_power"] = str(float(state["PowerConsumedWatts"]) -
                                   float(state["FanTotalPowerWatts"]))
        return True
    return False


def _get_cpu_info(state):
    state["cpu_percent"] = stool.get_current_cpu_usage()
    state["cpu_freq"] = stool.get_current_cpu_freq()


def _sys_prober():
    state = {
        "Time": time.strftime("%Y-%m-%d %H-%M-%S", time.localtime()),
        "PowerConsumedWatts": None,
        "FanTotalPowerWatts": None,
        "FanSpeedPercent": None,
        "InletTemp": None,
        "other_power": None,
        "cpu_temp": None,
        "cpu_percent": None,
        "cpu_freq": None,
    }

    _get_cpu_info(state)
    if not _get_sys_state_from_bmc(state):
        return False, None
    logging.debug("Sys state: %s", state)
    return True, state


def _check_result_and_state(result, state):
    if not result:
        logging.error("Collecting data failed.")
        return False
    if state["cpu_percent"] > CST.CPU_USAGE_STOP_TH:
        logging.warning("Collecting stoped. cpu_percent:%0.1f%%",
                        state["cpu_percent"])
        return False
    if float(state["cpu_temp"]) > 90:
        logging.warning("Collecting stoped. cpu_temp:%d", state["cpu_temp"])
        return False
    return True


def _do_collect_stable_thermal_data(interval):
    collected_data = pd.DataFrame()
    if not FanController.set_manual_mode():
        logging.error("Set Fan to manual mode failed.")
        return False, None
    num_speed = [85, 60, 40, 30]
    result = True
    for speed in num_speed:
        FanController.set_speed_by_duty(speed)
        logging.debug("Set fan to speed: %d%%", speed)
        temp_his = Queue(60)
        while True:
            time.sleep(interval)
            ret, state = _sys_prober()
            if not _check_result_and_state(ret, state):
                result = False
                break
            temp_his.push(float(state["cpu_temp"]))
            if temp_his.is_stable():
                break

        if result:
            for _ in range(10):
                time.sleep(interval)
                ret, state = _sys_prober()
                if not _check_result_and_state(ret, state):
                    result = False
                    break
                state_df = pd.DataFrame(state, index=[0, ])
                collected_data = pd.concat([collected_data, state_df], ignore_index=True)
        if not result:
            break

    FanController.set_auto_mode()
    return result, collected_data


# ========================================================================

def collect_stable_thermal_data(interval):
    """ Collecting stable thermal data under PERFORMANCE mode"""
    orgin_gov = stool.get_current_freq_governor()
    logging.debug("Current freq governor: %s", orgin_gov)
    if orgin_gov != "performance":
        stool.set_sys_to_performance_mode()

    result, stable_data = _do_collect_stable_thermal_data(interval)

    stool.set_current_freq_governor(orgin_gov)
    return result, stable_data
