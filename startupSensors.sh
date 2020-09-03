#!/bin/bash

# Esquema del Sensor: id tipoSensor puerto tiempo inicial final 1
#
#competing sensors
./sensorx 1 2 2378 1000 30 80 1  &
./sensorx 3 1 8821 1000 29 76 1  &
./sensorx 4 1 9876 1000 38 100 1  &
./sensorx 5 1 7632 1000 21 89 1  &
./sensorx 6 2 8873 1000 42 96 1  &
./sensorx 10 3 2234 1000 30 45 1  &
./sensorx 12 2 9081 1000 23 98 1  &
#complementary sensors
./sensorx 8 34 6732 1000 14 25 1  &
./sensorx 14 18 7123 1000 29 68 1  &
./sensorx 7 9 6903 1000 28 102 1  &
./sensorx 15 2 7094 1000 28 102 1  &

