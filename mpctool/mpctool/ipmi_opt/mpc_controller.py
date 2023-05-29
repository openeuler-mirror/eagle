# -*- coding: utf-8 -*-
# Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
"""
Communicate with BMC
"""

import os
import struct
import re
import logging
from mpctool.common.sys_tool import exec_shell_cmd
from mpctool.common import mpc_const as CST

SUCCUSS_RSP_FLAG = "db0700"


def _insert_hex_flag(src_string):
    string_list = re.findall(".{2}", src_string)
    return "0x" + " 0x".join(string_list)


# ==============================================================================

CMD_DATA_COLL = "ipmitool raw 0x30 0x92 0xdb 0x07 0x00 0x27 0x01 0x00"
NORMAL_STATE_FLAG = "00"  # The FLAG of normal state from BMC
SYS_EXP_FLAG = "fe"  # The FLAG of system exception state


def get_sys_state(content):
    """
    Request: collecting current state
    Response format: db 07 00 44 01 1c 28 22 00 28 00
    Bytes descripton:
    1       Completion code
    2:4     flag. fixed: OXdb0700
    5:6     Total power
    7       Inlet Temperature
    8       CPU Temperature
    9:10    Fan total power
    11      Fan Speed Percentage
    12      Status. 00: normal  ff: data exception  fe: system exception
    """
    rsp = exec_shell_cmd(CMD_DATA_COLL).replace(" ", "").lower()
    logging.debug("get_sys_state. cmd: %s, rsp: %s", CMD_DATA_COLL, rsp)
    if SUCCUSS_RSP_FLAG not in rsp:
        logging.error("get_sys_state command failure by ipmitool.")
        return None
    status = rsp[-2:]
    if status != CST.BMC_STATE_NORMAL:
        logging.warning("Exception happens during collection. status:%s", status)
        return status

    base = rsp.index(SUCCUSS_RSP_FLAG) + len(SUCCUSS_RSP_FLAG)
    content["PowerConsumedWatts"] = int(
        rsp[base + 2: base + 4] + rsp[base: base + 2], 16)
    base = base + 4
    content["InletTemp"] = int(rsp[base: base + 2], 16)
    base = base + 2
    content["cpu_temp"] = int(rsp[base: base + 2], 16)
    base = base + 2
    content["FanTotalPowerWatts"] = int(
        rsp[base + 2: base + 4] + rsp[base: base + 2], 16)
    base = base + 4
    content["FanSpeedPercent"] = int(rsp[base: base + 2], 16)
    return CST.BMC_STATE_NORMAL


# ipmitool Check if there is device change
CMD_CHECK_DEVICE_CHANGED = "ipmitool raw 0x30 0x92 0xdb 0x07 0x00 0x27 0x01 0x01"
NEED_RETRAINING_FLAG = "01"  # Indicates that the device is changed.


def get_device_changed_event():
    """
    Request: Query if training is required.
    Response format: db 07 00 01 00 01 00
    Bytes descripton:
    1       Completion code
    2:4     flag. fixed: OXdb0700 
    5:6     Version ID
    7       Retraining status. 
                01H: retraining is required. 
                00H: retraining is not required.
    """
    rsp = exec_shell_cmd(CMD_CHECK_DEVICE_CHANGED).replace(" ", "").lower()
    logging.debug("get_device_changed_event. cmd: %s, rsp: %s",
                  CMD_CHECK_DEVICE_CHANGED, rsp)
    if SUCCUSS_RSP_FLAG not in rsp:
        logging.error("get_device_changed_event command failure by ipmitool.")
        return None
    if rsp[10: 12] == NEED_RETRAINING_FLAG:
        sindex = rsp.index(SUCCUSS_RSP_FLAG) + len(SUCCUSS_RSP_FLAG)
        return int(rsp[sindex + 2: sindex + 4] + rsp[sindex: sindex + 2], 16)
    else:
        return None


def check_mpc_enabled():
    """ Check if the MPC function is enabled """
    rsp = exec_shell_cmd(CMD_CHECK_DEVICE_CHANGED).replace(" ", "").lower()
    logging.debug("check_mpc_enabled. cmd: %s, rsp: %s",
                  CMD_CHECK_DEVICE_CHANGED, rsp)
    if SUCCUSS_RSP_FLAG not in rsp:
        logging.error("check_mpc_enabled command failure by ipmitool.")
        return False
    return True


# ipmitool Sending the model params to the BMC.
# only the TR model is needed currently.
CMD_SEND_MODEL_PREFIX = "ipmitool raw 0x30 0x92 0xdb 0x07 0x00 0x26 0x01 "
MODEL_TYPE = "0x04 "
BYTE_STEP = 256
UPDATE_SUCCESS_FLAG = "00"


def send_model_to_bmc(change_version, inlet_temp, stable_power, lowest_fanspeed, model_data):
    """
    Request: Send model to BMC.
    Data Bytes after CMD_SEND_MODEL_PREFIX:
    1       data len
    2:3     change version id
    4       model type. fixed: 0x04 for TR model
    5       inlet temp
    6:7     stable power
    8:N     model data
    N+1     lowest fan speed

    Response format: db 07 00
    Rsp Bytes descripton:
    1       Completion code
    2:4     flag. fixed: OXdb0700
    5       status. 00: Succeed ff: failed
    """
    logging.debug("change_version: %d, inlet_temp: %0.2f, stable_power: %0.2f, lowest_fanspeed:%0.2f, model_data:%s",
                  change_version, inlet_temp, stable_power, lowest_fanspeed, model_data)
    print("Model Params: change_version:", change_version, "inlet_temp:",
          round(inlet_temp, 2), "lowest_fanspeed:", lowest_fanspeed, "model_data:", model_data)

    if change_version > BYTE_STEP * BYTE_STEP or \
            change_version <= -(BYTE_STEP * BYTE_STEP):
        logging.error("Invalid change_version: %d", change_version)
        return False
    change_version = int(change_version)
    change_version_str = _insert_hex_flag(struct.pack("H", change_version).hex()) + " "

    if inlet_temp > BYTE_STEP or inlet_temp < -(BYTE_STEP - 1):
        logging.error("Invalid inlet_temp: %d", inlet_temp)
        return False
    inlet_temp = int(inlet_temp)

    if stable_power > BYTE_STEP * BYTE_STEP or \
            stable_power <= -(BYTE_STEP * BYTE_STEP):
        logging.error("Invalid stable_power: %d", stable_power)
        return False
    stable_power = int(stable_power)
    stable_power_str = _insert_hex_flag(struct.pack("H", stable_power).hex()) + " "

    data_len = 6 + 8 * len(model_data) + 1
    if data_len > BYTE_STEP or data_len <= 4:
        logging.error("Invalid data_len: %d", data_len)
        return False
    data_str = " ".join([_insert_hex_flag(struct.pack("d", param).hex()) for param in model_data])

    if lowest_fanspeed > BYTE_STEP or lowest_fanspeed < -(BYTE_STEP - 1):
        logging.error("Invalid lowest_fanspeed: %d", lowest_fanspeed)
        return False
    lowest_fanspeed = int(lowest_fanspeed)

    cmd = CMD_SEND_MODEL_PREFIX + str(hex(data_len)) + " " + change_version_str + MODEL_TYPE \
          + str(hex(inlet_temp)) + " " + stable_power_str + data_str + " " + str(hex(lowest_fanspeed))

    rsp = exec_shell_cmd(cmd).replace(" ", "").lower()
    logging.debug("send_model_to_bmc. cmd: %s, rsp: %s", cmd, rsp)
    if SUCCUSS_RSP_FLAG not in rsp:
        logging.error("send_model_to_bmc command failure by ipmitool.")
        return False
    return bool(rsp[-2:] == UPDATE_SUCCESS_FLAG)
