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
    content = {}
    ret = mpcc.get_sys_state(content)

    if ret == CST.BMC_STATE_NORMAL:
        state["PowerConsumedWatts"] = content["PowerConsumedWatts"]
        state["FanTotalPowerWatts"] = content["FanTotalPowerWatts"]
        state["FanSpeedPercent"] = content["FanSpeedPercent"]
        state["InletTemp"] = content["InletTemp"]
        state["cpu_temp"] = content["cpu_temp"]
        state["other_power"] = str(float(state["PowerConsumedWatts"]) -
                                   float(state["FanTotalPowerWatts"]))
    return ret


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
    ret = _get_sys_state_from_bmc(state)
    logging.debug("Sys state: %s, ret:%s", state, ret)
    return ret, state


def _check_cpu_usage(usage):
    if usage > CST.CPU_USAGE_STOP_TH:
        logging.warning("Collecting stoped. cpu_percent:%0.1f%%", usage)
        return False
    return True


MAX_UNSTABLE_DATA = 200


def _loop_to_stable_state(interval, is_last_speed):
    unstable_data = pd.DataFrame()
    temp_his = Queue(60)
    while True:
        time.sleep(interval)
        ret, state = _sys_prober()

        if not _check_cpu_usage(state["cpu_percent"]):
            unstable_data.drop(unstable_data.index, inplace=True)
            return CST.COLL_STATE_INVALID_SHORT, unstable_data

        if ret == CST.BMC_STATE_NORMAL:
            if is_last_speed:
                state_df = pd.DataFrame(state, index=[0, ])
                unstable_data = pd.concat([unstable_data, state_df], ignore_index=True)
                if len(unstable_data) > MAX_UNSTABLE_DATA:
                    unstable_data.drop(unstable_data.index[0], inplace=True)
            temp_his.push(float(state["cpu_temp"]))
            if temp_his.is_stable():
                unstable_data.drop(unstable_data.index, inplace=True)
                return CST.COLL_STATE_VALID, unstable_data
        else:
            logging.warning("Exception happens, the collecting stoped. ret: %s, is_last_speed: %d",
                            ret, is_last_speed)
            if is_last_speed:
                # keep the unstable_data
                return CST.COLL_STATE_VALID_NEED_FIT, unstable_data
            else:
                unstable_data.drop(unstable_data.index, inplace=True)
                return CST.COLL_STATE_INVALID_LONG, unstable_data


def _do_collect_stable_thermal_data(interval):
    '''
    When the fan speed is 30%, model training continues even if the device does not
    enter the stable state due to component overtemperature,
    but unsteady data needs to be fitted.
    '''
    collected_data = pd.DataFrame()
    unstable_data = pd.DataFrame()
    if not FanController.set_manual_mode():
        logging.error("Set Fan to manual mode failed.")
        return False, None
    num_speed = [85, 60, 40, 30]
    result = CST.COLL_STATE_VALID

    for speed in num_speed:
        FanController.set_speed_by_duty(speed)
        logging.debug("Set fan to speed: %d%%", speed)
        is_last_speed = speed == num_speed[-1]
        coll_state, unstable_data = _loop_to_stable_state(interval, is_last_speed)
        if coll_state != CST.COLL_STATE_VALID:
            result = coll_state
            break

        for _ in range(10):
            time.sleep(interval)
            ret, state = _sys_prober()
            if not _check_cpu_usage(state["cpu_percent"]):
                result = CST.COLL_STATE_INVALID_SHORT
                break
            if ret != CST.BMC_STATE_NORMAL:
                if is_last_speed:  # keep it valid if it is the last fan speed.
                    break
                result = CST.COLL_STATE_INVALID_LONG
                break

            state_df = pd.DataFrame(state, index=[0, ])
            collected_data = pd.concat([collected_data, state_df], ignore_index=True)
        if result != CST.COLL_STATE_VALID:
            break

    FanController.set_auto_mode()
    return result, collected_data, unstable_data


# ========================================================================

def collect_stable_thermal_data(interval):
    """ Collecting stable thermal data under PERFORMANCE mode"""
    orgin_gov = stool.get_current_freq_governor()
    logging.debug("Current freq governor: %s", orgin_gov)
    if orgin_gov != "performance":
        stool.set_sys_to_performance_mode()

    result, stable_data, unstable_data = _do_collect_stable_thermal_data(interval)

    stool.set_current_freq_governor(orgin_gov)
    return result, stable_data, unstable_data

def get_curr_inlet_temp():
    """
    Get the current inlet temperature
    Return:  result:bool, InletTemp: float
    """
    content = {}
    if mpcc.get_sys_state(content) == CST.BMC_STATE_NORMAL:
        return True, content["InletTemp"]
    return False, 0.0
