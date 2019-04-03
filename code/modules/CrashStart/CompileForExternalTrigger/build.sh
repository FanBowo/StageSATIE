#!/bin/sh
# This script is used to build the ig500Continuous on unix systems.
# Example: ./build.sh

gcc -no-pie -Wall main.c -IsbgCom/src sbgCom/libSbgComSerial.a -o continu_sync

