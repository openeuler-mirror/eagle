# -*- coding: utf-8 -*-
# Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
"""
mpctool
"""

import time
import os
import sys
import logging
from logging.handlers import RotatingFileHandler
import pandas as pd
import numpy as np
from mpctool.data_collector.running_data_collector import collect_stable_thermal_data
from mpctool.data_collector.running_data_collector import get_curr_inlet_temp
from mpctool.data_collector.data_processor import process_stable_training_data
from mpctool.data_collector.data_processor import predict_stable_power_and_temp
from mpctool.data_collector.data_processor import search_best_lowspeed
from mpctool.model_trainer.maintain_model import train_stable_model
from mpctool.model_trainer.maintain_model import train_power_func_model
from mpctool.model_trainer.maintain_model import initialize_model
from mpctool.common import mpc_const as CST
from mpctool.common import sys_tool as stool
from mpctool.ipmi_opt import mpc_controller as mpcc


class Tst(object):
    """ The current state of the task. """
    manual_mode = True
    long_term_sleep_times = 0
    sys_state = None
    change_version = None
    avg_inlet_temp = None
    avg_stable_power = None
    lowest_fanspeed = None
    collected_data = pd.DataFrame()
    unstable_data = pd.DataFrame()
    processed_data = pd.DataFrame()
    model_params = None


LOG_LEVELS = {
    "debug": logging.DEBUG,
    "info": logging.INFO,
    "warn": logging.WARN,
    "error": logging.ERROR,
    "critical": logging.CRITICAL
}


def _init_system():
    log_level = logging.WARN

    if CST.LOG_LEVEL in iter(LOG_LEVELS):
        log_level = LOG_LEVELS[CST.LOG_LEVEL]
    if not os.path.exists(CST.LOG_FILE_PATH):
        os.makedirs(CST.LOG_FILE_PATH)
    loghd = RotatingFileHandler(
        filename=CST.LOG_FILE_FILENAME_M if Tst.manual_mode else CST.LOG_FILE_FILENAME,
        mode="a",
        maxBytes=CST.LOG_FILE_SIZE,
        backupCount=CST.LOG_FILE_COUNT - 1,
        encoding=None, delay=0
    )
    logging.basicConfig(format="%(asctime)s %(levelname)-8s %(message)s",
                        level=log_level, handlers=[loghd])

    logging.info("System init ...=========================================")
    if not stool.check_ipmitool():
        logging.critical("Can not find ipmitool, system will exit.")
        return False
    if not mpcc.check_mpc_enabled():
        logging.critical("MPC function is not enabled on this machine, system will exit.")
        return False

    pd.set_option("display.max_columns", None)
    pd.set_option("display.max_rows", None)

    logging.info("System init succeed.")
    return True


def _update_sys_state():
    """
    Check whether there is a new device change.
    If there is a change, train from the beginning.
    In manual execution mode, training is required regardless of whether there is any change.
    """
    change_version = mpcc.get_device_changed_event()
    if Tst.change_version is None:  # no task right now
        if change_version is None:
            if Tst.manual_mode:
                Tst.change_version = 0  # 0 is a special version id indicates to manual mode
                Tst.sys_state = CST.SYS_STATE_COLLECTING
            return
        logging.info("Get change event from BMC. version: %d", change_version)
        Tst.change_version = change_version
        Tst.sys_state = CST.SYS_STATE_COLLECTING
        return

    if change_version is None:  # has task, but no change.
        return
    if Tst.change_version != change_version:  # has newer change
        logging.info("Get newer change event from BMC. old version: %d new version: %d",
                     Tst.change_version, change_version)
        Tst.change_version = change_version
        Tst.sys_state = CST.SYS_STATE_COLLECTING


def _loop_to_could_start_collecting():
    '''
    Check the system status to ensure that the collection conditions are met.
    1. CPU usage is lower than CPU_USAGE_SEC_TH for at least
       CHECK_UPDATE_INTERVAL * LOW_USAGE_LASTING_TIMES_TH seconds.
    2. The inlet temperature is not higher than MAX_ENV_TEMP.
    '''
    cond_met_lasting_times = 0
    while True:
        time.sleep(CST.CHECK_UPDATE_INTERVAL)

        if not stool.check_cpu_usage(CST.CPU_USAGE_SEC_TH):
            logging.warning("The CPU usage exceeds the threshold: %0.1f%%",
                          CST.CPU_USAGE_SEC_TH)
            cond_met_lasting_times = 0
            continue

        ret, inlet_t = get_curr_inlet_temp()
        if not ret or inlet_t > CST.MAX_ENV_TEMP:
            logging.warning("Get inlet temp failed or the inlet temp exceeds the threshold: %0.1f%%, ret:%d",
                          CST.MAX_ENV_TEMP, ret)
            cond_met_lasting_times = 0
            continue

        if cond_met_lasting_times < CST.LOW_USAGE_LASTING_TIMES_TH:
            cond_met_lasting_times += 1
        else:
            break


def _preprocess_unstable_data():
    if len(Tst.unstable_data) > 0:
        ret, fitted_fd = predict_stable_power_and_temp(Tst.unstable_data)
        if not ret:
            logging.error("Predict stable power failed.")
            return False
        Tst.collected_data = pd.concat([Tst.collected_data, fitted_fd], ignore_index=True)
    return True

def _collect_data():
    """
    step1: Collecting data
    """
    Tst.collected_data = pd.DataFrame()
    Tst.unstable_data = pd.DataFrame()
    result, Tst.collected_data, Tst.unstable_data = collect_stable_thermal_data(CST.COLLECTING_INTERVAL)
    if result != CST.COLL_STATE_VALID and result != CST.COLL_STATE_VALID_NEED_FIT:
        Tst.collected_data = pd.DataFrame()
        Tst.unstable_data = pd.DataFrame()
    logging.debug("Collected data:\n%s", Tst.collected_data)
    logging.debug("Unstable data:\n%s", Tst.unstable_data)
    return result


def _train_models():
    """
    step2: Training model.
    """
    if not _preprocess_unstable_data():
        return False

    intervals = [1, 1]
    hwdata_cols = ["cpu_freq", "cpu_percent"]
    bmc_cols = [
        "PowerConsumedWatts",
        "FanTotalPowerWatts",
        "FanSpeedPercent",
        "InletTemp",
        "other_power",
        "cpu_temp"
    ]
    temp_diff = 10
    Tst.avg_inlet_temp, Tst.avg_stable_power, Tst.processed_data = \
        process_stable_training_data(Tst.collected_data, hwdata_cols, bmc_cols,
                                     intervals, temp_diff)
    logging.debug("Processed data:\n%s", Tst.processed_data)
    if np.isnan(Tst.avg_inlet_temp) or np.isnan(Tst.avg_stable_power):
        logging.error("Invalid data. inlet_temp:%0.2f, stable_data:%0.2f",
                      Tst.avg_stable_power, Tst.avg_inlet_temp)
        return False
    if Tst.avg_inlet_temp > CST.MAX_ENV_TEMP:
        logging.error("Average Environment Temp is out of range[%0.1f, %0.1f]. inlet_temp:%0.2f",
                      CST.MIN_ENV_TEMP, CST.MAX_ENV_TEMP, Tst.avg_inlet_temp)
        return False

    feature = ["FanSpeedPercent"]
    output = "thermal_resistence"
    model_degree = 1
    power_degree = 1
    model = initialize_model("power", model_degree, power_degree)
    best_power_degree = train_power_func_model(
        Tst.processed_data, model, feature, output)
    model = initialize_model("single", model_degree, best_power_degree)
    train_stable_model(Tst.processed_data, model, feature, output)
    Tst.model_params = model.get_model_params()

    Tst.lowest_fanspeed = search_best_lowspeed(Tst.collected_data)
    logging.info("Training result: inlet_temp: %0.1f, stable_power: %0.1f, lowest_fanspeed:%0.1f, model_params: %s",
                 Tst.avg_inlet_temp, Tst.avg_stable_power, Tst.lowest_fanspeed, Tst.model_params)
    return True


def _send_model_params():
    """
    step3：Sending the traided result to the BMC
    """
    for _ in range(3):
        if mpcc.send_model_to_bmc(Tst.change_version, Tst.avg_inlet_temp,
                                  Tst.avg_stable_power, Tst.lowest_fanspeed, Tst.model_params):
            return True
        else:
            time.sleep(CST.CHECK_UPDATE_INTERVAL)
    # Need to be trained again if sending failed for 3 times due to the invalid data
    Tst.sys_state = CST.SYS_STATE_COLLECTING
    return False


# ========================================================================

def main():
    """ Control the system state machine """
    argc = len(sys.argv)
    Tst.manual_mode = True
    if argc >= 2:
        if sys.argv[1] == "auto":
            Tst.manual_mode = False

    if not _init_system():
        sys.exit(2)
    logging.info("System run in %s mode", "MANUAL" if Tst.manual_mode else "AUTO")

    Tst.sys_state = CST.SYS_STATE_NONE
    while True:
        time.sleep(CST.CHECK_UPDATE_INTERVAL)
        _update_sys_state()
        if Tst.sys_state == CST.SYS_STATE_NONE:
            continue
        if Tst.long_term_sleep_times > CST.MAX_LONG_TERM_SLEEP_TIMES:
            logging.error("System exit due to long_term_sleep over %d times",
                          CST.MAX_LONG_TERM_SLEEP_TIMES)
            sys.exit(3)

        # step1. Collecting data
        if Tst.sys_state == CST.SYS_STATE_COLLECTING:
            logging.info("step1: start collecting, this may last 20 minutes...")
            print("step1: start collecting, this may last 20 minutes...")
            _loop_to_could_start_collecting()
            _update_sys_state()
            result = _collect_data()
            if result == CST.COLL_STATE_VALID or result == CST.COLL_STATE_VALID_NEED_FIT:
                Tst.sys_state = CST.SYS_STATE_TRAINING
                _update_sys_state()
            elif result == CST.COLL_STATE_INVALID_LONG:
                Tst.long_term_sleep_times += 1
                logging.warning("Collecting data failed. Try again %d minutes later.", CST.LONG_TERM_SLEEP_TIME / 60)
                time.sleep(CST.LONG_TERM_SLEEP_TIME)
                continue
            else:
                logging.warning("Collecting data failed. Try again %d seconds later.", CST.CHECK_UPDATE_INTERVAL)
                continue
            Tst.long_term_sleep_times = 0

        # step2: Training model
        if Tst.sys_state == CST.SYS_STATE_TRAINING:
            logging.info("step2: Start training......")
            print("step2: Start training......")
            if _train_models():
                Tst.sys_state = CST.SYS_STATE_SEND_MODEL
            else:
                logging.warning("Training model failed. Try again later.")
                Tst.sys_state = CST.SYS_STATE_COLLECTING
                continue

        # step3: Sending model params
        if Tst.sys_state == CST.SYS_STATE_SEND_MODEL:
            logging.info("step3: Start sending model params......")
            print("step3: Start sending model params......")
            if not _send_model_params():
                logging.warning("Sending model params to BMC failed. System will try again later")
                print("Sending model params to BMC failed. System will try again later.")
                continue

            logging.info("Training model for version %d succeed.",
                         Tst.change_version)
            if Tst.manual_mode:  # Run once in manual mode
                print("Training model for version",
                      Tst.change_version, "succeed.")
                sys.exit()
            Tst.sys_state = CST.SYS_STATE_NONE
            Tst.change_version = None


if __name__ == "__main__":
    main()
