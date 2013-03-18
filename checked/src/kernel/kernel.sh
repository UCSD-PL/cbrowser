#!/usr/bin/env bash

export LD_LIBRARY_PATH=${PWD}/../../../trusted/src/kernel:${PWD}/../../../trusted/src/msg/:${PWD}/../../../libs/libsoup-2.34.0/libsoup/.libs 

stty cbreak
../../../trusted/src/procs/input.py | ./kernel 
