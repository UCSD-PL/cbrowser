#!/usr/bin/env bash

ROOT="${PWD}/../../"
CSOLVE_DIR=/home/abakst/scratch/csolve

export LD_LIBRARY_PATH="${ROOT}/trusted/src/msg"
export LIBS="-lmsg" 
export LDFLAGS="-L${ROOT}/trusted/src/msg" 
export CFLAGS="-I${ROOT}/trusted/inc -I $CSOLVE_DIR/lib" 

if [[ -f "${ROOT}/trusted/src/msg/libmsg.so" ]]; then
	echo 'Found libmsg.so, continuing'
else
	echo 'Please build libmsg.so before continuing'
	echo "Execute make in ${ROOT}"
	exit 1
fi	

# ./configure && make 
make
