# -*- coding: utf-8 -*-
# Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
"""
Model control
"""

import logging
import numpy as np
from mpctool.model_trainer import models


def mape(actual, pred):
    actual, pred = np.array(actual).squeeze(), np.array(pred)
    mape_err = np.divide(np.abs(actual - pred), actual)
    return np.mean(mape_err)


def r2_score(actual, pred):
    actual, pred = np.array(actual).squeeze(), np.array(pred)
    sse = np.sum((actual - pred) ** 2)
    sst = np.sum((actual - np.mean(actual)) ** 2)
    if sst != 0:
        return 1 - sse / sst
    return 0


def show_results(model, x, y):
    y_predict = model.predict(x)
    mape_err = mape(y, y_predict)
    r2 = r2_score(y, y_predict)
    logging.info("Model error value: [mean_relative_error: %0.5f  R-square_value: %0.5f]",
                 round(mape_err, 5), round(r2, 5))


def show_power_func_results(model, x, y, n):
    y_predict = model.predict(x, n)
    mape_err = mape(y, y_predict)
    return mape_err


def load_training_data(data, cols, output):
    return data[cols], data[output]


# ====================================================================================

def initialize_model(model_type, model_degree, power_degree):
    """ Model initialization """
    if model_type.lower() == "single":
        return models.LinearModel(model_degree, power_degree)
    if model_type.lower() == "power":
        return models.PowerFuncTestModel(model_degree)
    return None


def train_stable_model(processed_data, model, features, output):
    """train stable model"""
    x, y = load_training_data(processed_data, features, output)
    model.train(x, y.values.ravel())
    show_results(model, x, y)


def train_power_func_model(processed_data, model, features, output):
    """train power function model. return: float"""
    x_train, y_train = load_training_data(processed_data, features, output)
    power_degree = [-i / 10 for i in range(1, 21)]
    best_degree = -0.1
    best_err = 10000
    for n in power_degree:
        model.train(x_train, y_train.values.ravel(), n)
        err = show_power_func_results(model, x_train, y_train, n)
        if err < best_err:
            best_degree = n
            best_err = err
    return best_degree
