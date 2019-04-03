#!/bin/sh
# This script is used to build the ironCalibExample on unix systems.
# Example: ./build.sh

gcc -no-pie -Wall ../../src/main.c  -I../../../sbgIronCalibration/src/ ../../sbgIronCalibration/sbgIronCalibration.dll -o ../../ironCalibExample

