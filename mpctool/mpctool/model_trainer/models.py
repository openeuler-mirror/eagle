# -*- coding: utf-8 -*-
# Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
"""
Models
"""
import numpy as np


class LinearModel():
    """ Linear model. Tag：single """

    def __init__(self, degree=2, power_degree=1):
        self._degree = degree
        self._power_degree = power_degree
        self._coef = None

    def train(self, x, y):
        x_processed = x ** self._power_degree
        self._coef = np.polyfit(x_processed.values.flatten(), y, self._degree)

    def predict(self, x):
        x_processed = x ** self._power_degree
        return np.polyval(self._coef, x_processed).squeeze()

    def get_model_params(self):
        params = []
        params.extend(self._coef)
        params.append(self._power_degree)
        return params


class PowerFuncTestModel():
    """ Power model. Tag: power"""

    def __init__(self, degree=1):
        self._degree = degree
        self._coef = None

    def train(self, x, y, n):
        x_processed = x ** n
        self._coef = np.polyfit(x_processed.values.flatten(), y, self._degree)

    def predict(self, x, n):
        x_processed = x ** n
        return np.polyval(self._coef, x_processed.values.flatten()).squeeze()
