#!/usr/bin/env python3

import numpy as np

reference = 5.0

def curve(x):
    return -1814.9*x**3+259.56*x**2+14.147*x+0.2954

def get_voltage(x):
    return (x-0.3)*5.0

def get_value(v):
    return ((v+0.3)*1024)/reference

def inverse(x):
    return 1/x-0.42

mapping = dict()

for i in np.linspace(-0.01, 0.12, 2048):
    voltage = curve(i)
    distance = int(10*inverse(i))
    value = int(get_value(voltage))

    if distance < 0:
        continue

    if voltage > 3:
        continue

    if value < 0 or value > 1023:
        continue

    if value in mapping:
        continue

    mapping[value] = distance

for i in range(1024):
    if not i in mapping:
        value = -1
    else:
        value = mapping[i]
    print("{:6d}, # {:4d}".format(value, i))
