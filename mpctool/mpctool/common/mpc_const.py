# -*- coding: utf-8 -*-
# Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
"""
Constant
"""

# system state
SYS_STATE_NONE = 0          # No training task.
SYS_STATE_COLLECTING = 1    # Has training task which is in Collecting Step
SYS_STATE_TRAINING = 2      # Has training task which is in Training Step
SYS_STATE_SEND_MODEL = 3    # Has training task which is in Sending Model Pramas Step

# system config
# The collection can be started only when the CPU usage is continuously
#    less than CPU_USAGE_SEC_TH.
# Low load duration: LOW_USAGE_LASTING_TIMES_TH * CHECK_UPDATE_INTERVAL
# Once the CPU usage is greater than CPU_USAGE_STOP_TH during data collection,
#    data collection will be stopped.
CPU_USAGE_SEC_TH = 1        # percentage
CPU_USAGE_STOP_TH = 1.5     # percentage
CHECK_UPDATE_INTERVAL = 10  # seconds. Interval for retry loop.
LOW_USAGE_LASTING_TIMES_TH = 6
COLLECTING_INTERVAL = 1     # seconds. Interval for collecting.

# The training result is valid only when the environment temperature
#    is between [MIN_ENV_TEMP, MAX_ENV_TEMP].
MIN_ENV_TEMP = 23.0         # Min environment temperature
MAX_ENV_TEMP = 28.0         # Max environment temperature

# log
LOG_FILE_PATH = "/var/log/mpctool"
LOG_FILE_FILENAME = LOG_FILE_PATH + "/mpctool.log"
LOG_FILE_FILENAME_M = LOG_FILE_PATH + "/mpctool_m.log"
LOG_LEVEL = "info"     # debug, info, warn, error, critical
LOG_FILE_COUNT = 3
LOG_FILE_SIZE = 5 * 1024 * 1024     # bytes
