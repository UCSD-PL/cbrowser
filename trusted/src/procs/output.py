#!/usr/bin/env python

# Quark is Copyright (C) 2012-2015, Quark Team.
#
# You can redistribute and modify it under the terms of the GNU GPL,
# version 2 or later, but it is made available WITHOUT ANY WARRANTY.
#
# For more information about Quark, see our web site at:
# http://goto.ucsd.edu/quark/


import sys
import os
import tempfile
import msg
import socket
import shm
import opt
import threading
import time
import struct
import array
import traceback
import cPickle as pickle
import signal

def olog(str):
    olog_nonl(str + "\n")

def olog_nonl(str):
    sys.stderr.write("O: " + str)
    sys.stderr.flush()

def test_loop(soc):
    while True:
        m = msg.read_message_soc(soc)
        olog("Received message: %s" % str(m))

def signal_handler(signal, frame):
    sys.exit(0)

def main():
    signal.signal(signal.SIGINT, signal_handler)
    try:
        soc_fd = int(sys.argv[1])
        soc = socket.fromfd(soc_fd, socket.AF_UNIX, socket.SOCK_STREAM)
        olog("New UI listening on socket fd %d" % int(sys.argv[1]))
        test_loop(soc)
    except Exception as e:
        olog("New UI died: %s" % str(e))
        print traceback.format_exc()


main()

