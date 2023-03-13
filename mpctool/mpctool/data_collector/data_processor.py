# -*- coding: utf-8 -*-
# Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
"""
Process the collected data
"""

import copy
import numpy
import pandas as pd


def process_stable_training_data(collected_data, hwdata_cols, bmc_cols, intervals, temp_diff):
    """ Process the collected data. return (average inlet temp, average power, processed data list)"""
    data_sel = {}
    info = collected_data[collected_data.columns.dropna()]

    for col in hwdata_cols + bmc_cols:
        data_sel[col] = copy.deepcopy(info[col])

    data_sel["pred_cpu_temp"] = copy.deepcopy(data_sel["cpu_temp"])
    data_sel["pred_cpu_temp"][:-intervals[1] // intervals[0]] = \
        copy.deepcopy(data_sel["cpu_temp"][intervals[1] // intervals[0]:])

    df = pd.DataFrame(data_sel)
    for col in hwdata_cols:
        df = df.drop(df[df[col] < 0].index)
        df.dropna(axis=0, how="any", inplace=True)
    for col in bmc_cols:
        df = df.drop(df[df[col] == "None"].index)
    df = df.drop(df[df["pred_cpu_temp"] == "None"].index)

    df_ = df[list(data_sel.keys())].apply(pd.to_numeric, errors="coerce")

    df_ = df_.drop(df_[abs(df_["pred_cpu_temp"] - df_["cpu_temp"]) >= temp_diff].index)
    df_ = df_.drop(df_[df_["other_power"] < 1].index)
    df_ = df_.drop(df_[df_["other_power"] > 10000].index)

    other_power = list(df_["other_power"].values)
    for ind in range(1, len(other_power) - 1):
        if abs(other_power[ind] - other_power[ind - 1]) / other_power[ind - 1] > 0.3 and \
                abs(other_power[ind] - other_power[ind + 1]) / other_power[ind + 1] > 0.3:
            df_ = df_.drop(df_[df_["other_power"] == other_power[ind]].index)

    df_["thermal_resistence"] = (df_["cpu_temp"] - df_["InletTemp"]) / df_["other_power"]
    df_for_stable_power = df_[df_["FanSpeedPercent"] >= 80]
    return numpy.average(df_["InletTemp"]), \
        numpy.average(df_for_stable_power["PowerConsumedWatts"]), df_
