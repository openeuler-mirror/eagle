# -*- coding: utf-8 -*-
# Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
"""
Process the collected data
"""

import copy
import numpy as np
import pandas as pd
from scipy.optimize import curve_fit
from scipy.optimize import fmin


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
    return np.average(df_["InletTemp"]), \
        np.average(df_for_stable_power["PowerConsumedWatts"]), df_

SCALE_LEVEL = 10

def _func(x, a, b, c):
    return a * np.exp(- b * x) + c


def predict_stable_power_and_temp(dataset):
    """
    Function: Predict the stable power by a curve function which is fitted by the unstable data.
    Input: dataset: DataFrame. the collected unstable data
    Output: the prediction result and the stable power data.
    """
    df = dataset
    start_index = df.index[0]
    end_index = df.index[-1]
    max_power = df.loc[end_index, 'PowerConsumedWatts']
    max_temp = df.loc[end_index, 'cpu_temp']

    data_dict = {'time': [], 'power': [], 'temp': []}
    for i, val in enumerate(range(start_index, end_index)):
        if val == start_index:
            continue
        # scale down the index to avoid overflow warning during curve fitting.
        data_dict['time'].append(i / SCALE_LEVEL)
        # To ensure that the trend curve can be fitted (increasing monotonically),
        # the data needs to be processed by subtracting the original data
        data_dict['power'].append(min(max_power - df.loc[val, 'PowerConsumedWatts'], \
                                      data_dict['power'][-1] if data_dict['power'] else max_power))
        data_dict['temp'].append(min(max_temp - df.loc[val, 'cpu_temp'], \
                                      data_dict['temp'][-1] if data_dict['temp'] else max_temp))
    train_df = pd.DataFrame(data_dict)

    try:
        popt_p, _ = curve_fit(_func, train_df['time'], train_df['power'])
        popt_t, _ = curve_fit(_func, train_df['time'], train_df['temp'])
    except Exception:
        return False, pd.DataFrame()
    #len * 2 is A large enough value X which is used for computing the stable data Y.
    x = len(train_df) * 2 / SCALE_LEVEL
    stable_power = max_power - _func(x, popt_p[0], popt_p[1], popt_p[2])
    stable_temp = max_temp - _func(x, popt_t[0], popt_t[1], popt_t[2])
    ret_df = df.tail(1)
    ret_df.at[ret_df.index[0], "PowerConsumedWatts"] = stable_power
    ret_df.at[ret_df.index[0], "cpu_temp"] = stable_temp
    ret_df = pd.DataFrame(ret_df.values.tolist(), index=np.arange(10), columns=ret_df.columns)
    return True, ret_df

MIN_FAN_SPEED = 30  # 30 the min fan speed for collection.
MAX_FAN_SPEED = 85  # 85 the max fan speed for collection.

def search_best_lowspeed(collected_data):
    data_sel = {}
    info = collected_data[collected_data.columns.dropna()]
    data_sel['FanSpeedPercent'] = copy.deepcopy(info['FanSpeedPercent'])
    data_sel['PowerConsumedWatts'] = copy.deepcopy(info['PowerConsumedWatts'])
    df = pd.DataFrame(data_sel)
    df = df.drop(df[df['FanSpeedPercent'] == 'None'].index)
    df = df.drop(df[df['PowerConsumedWatts'] == 'None'].index)

    df = df.apply(pd.to_numeric, errors='coerce')

    speedgap0 = min(df['FanSpeedPercent']) - MIN_FAN_SPEED
    speedgap1 = max(df['FanSpeedPercent']) - MAX_FAN_SPEED
    gradient = (speedgap1 - speedgap0) / (MAX_FAN_SPEED - MIN_FAN_SPEED)
    intercept = speedgap1 - gradient * MAX_FAN_SPEED
    speedgap = lambda x: round(gradient * x + intercept)

    # 3: Cubic equation fitting
    popt_p = np.polyfit(df['FanSpeedPercent'], df['PowerConsumedWatts'], 3)
    f = lambda x: popt_p[0]*x**3 + popt_p[1]*x**2 + popt_p[2]*x + popt_p[3]
    result = fmin(f, MIN_FAN_SPEED)

    return result - speedgap(result[0])
