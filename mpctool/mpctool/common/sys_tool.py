# -*- coding: utf-8 -*-
# Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
"""
Tool for system operations
"""

import os
import subprocess
import re
import logging
import psutil


def _read_file(file):
    try:
        with open(file, "r") as fs:
            return fs.read()
    except Exception as err:
        logging.error(err)
        return ""


def _write_file_w(file, content):
    try:
        with open(file, "w") as fs:
            fs.write(content)
            return True
    except Exception as err:
        logging.error(err)
        return False


def _regularize_unit(value):
    if 0 < value < 10:
        value = value * 1000
    elif value > 10000:
        value = value / 1000
    return int(value)


CPUFREQ_PATH = "/sys/devices/system/cpu/cpufreq/"
CPUFREQ_CPU0_PATH = "/sys/devices/system/cpu/cpu0/cpufreq/"
SYSFS_AVAILABLE_FREQUENCIES = CPUFREQ_CPU0_PATH + "scaling_available_frequencies"
SYSFS_MAX_FREQ = CPUFREQ_CPU0_PATH + "scaling_max_freq"
SYSFS_AVAILABLE_GOVERNORS = CPUFREQ_CPU0_PATH + "scaling_available_governors"
SYS_CURR_FREQ_GOV = CPUFREQ_CPU0_PATH + "scaling_governor"
PERFORMANCE_GOV = "performance"


def _get_max_cpufreq():
    if os.path.exists(SYSFS_AVAILABLE_FREQUENCIES):
        freq_str = _read_file(SYSFS_AVAILABLE_FREQUENCIES).strip().split(" ")
        return max([_regularize_unit(int(x)) for x in freq_str])
    else:
        return _regularize_unit(int(_read_file(SYSFS_MAX_FREQ).strip()))


def _set_freq_policy(attr, content):
    dirs = os.listdir(CPUFREQ_PATH)
    for dr in dirs:
        if "policy" in dr:
            path = os.path.join(CPUFREQ_PATH, dr, attr)
            _write_file_w(path, content)


# =========================================================================================
def exec_shell_cmd(cmd):
    """
    Function: execute shell cmd
    return: cmd result, type: string
    """
    return subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE).communicate()[0].decode().strip()


def get_current_cpu_usage():
    """return: cpu usage. type: float"""
    return psutil.cpu_percent(percpu=False)


def check_cpu_usage(threshold):
    """return false if the cpu usage is bigger than threshold"""
    return bool(get_current_cpu_usage() < threshold)


def check_ipmitool():
    """return false if the impitool is not installed"""
    result = exec_shell_cmd("ipmitool -V")
    return bool("version" in result)


def get_current_freq_governor():
    """ Return the current freq governro. type: string"""
    return _read_file(SYS_CURR_FREQ_GOV).strip()


def set_current_freq_governor(gov):
    """ Set the freq governor. return type: bool"""
    if gov in _read_file(SYSFS_AVAILABLE_GOVERNORS).strip().split(" "):
        _set_freq_policy("scaling_governor", gov)
        return True
    return False


def set_sys_to_performance_mode():
    """ Set the system to PERFORMACE mode"""
    if set_current_freq_governor(PERFORMANCE_GOV):
        return
    max_freq = _get_max_cpufreq()
    if max_freq:
        _set_freq_policy("scaling_cur_freq", str(max_freq * 1000))


def get_current_cpu_freq():
    """ Return the current cpu freq for cpu_id. Retrurn type: float"""
    return psutil.cpu_freq(percpu=False).current
