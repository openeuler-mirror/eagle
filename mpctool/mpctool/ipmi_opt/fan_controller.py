# -*- coding: utf-8 -*-
# Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
"""
Fan controller
"""

from mpctool.common.sys_tool import exec_shell_cmd

# ipmitool. Cmd prefix
CMD_SET_FAN_SPEED = "ipmitool raw 0x2c 0x15 0x02 0x00 "

# ipmitool. Cmd of SET FAN MODE
CMD_SET_FAN_MODE = [
    "ipmitool raw 0x30 0x91 0xdb 0x07 0x00 0x01 0x00",
    "ipmitool raw 0x30 0x91 0xdb 0x07 0x00 0x01 0x01 0x00 0x00 0x00 0x00"
]

FAN_SPEED = [
    "0x0a",  # 10%
    "0x0f",  # 15%
    "0x14",  # 20%
    "0x19",  # 25%
    "0x1e",  # 30%
    "0x23",  # 35%
    "0x28",  # 40%
    "0x2d",  # 45%
    "0x32",  # 50%
    "0x37",  # 55%
    "0x3c",  # 60%
    "0x41",  # 65%
    "0x46",  # 70%
    "0x4b",  # 75%
    "0x50",  # 80%
    "0x55",  # 85%
    "0x5a",  # 90%
    "0x5f",  # 95%
]

SUCCUSS_RSP_FLAG = "db0700"


class FanController():
    def __init__(self):
        self.num_speed = len(FAN_SPEED)

    @staticmethod
    def set_manual_mode():
        """Set the fan to manual mode"""
        rsp = exec_shell_cmd(CMD_SET_FAN_MODE[1]).replace(" ", "").lower()
        return bool(SUCCUSS_RSP_FLAG in rsp)

    @staticmethod
    def set_auto_mode():
        """set the fan to auto mode"""
        rsp = exec_shell_cmd(CMD_SET_FAN_MODE[0]).replace(" ", "").lower()
        return bool(SUCCUSS_RSP_FLAG in rsp)

    @staticmethod
    def set_speed_by_duty(duty):
        """Set fan speed"""
        allowed_duty = max(min(duty, 100), 10)
        exec_shell_cmd(CMD_SET_FAN_SPEED + str(hex(int(allowed_duty))))
