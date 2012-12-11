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
import cPickle as pickle

#gtk.gdk.threads_init()

def olog(str):
    olog_nonl(str + "\n")

def olog_nonl(str):
    sys.stderr.write("O: " + str)
    sys.stderr.flush()

def main():
    try:
        soc_fd = int(sys.argv[1])
        soc = socket.fromfd(soc_fd, socket.AF_UNIX, socket.SOCK_STREAM)
        tlog("New UI listening on socket fd %d" % int(sys.argv[1]))
    except:
        tlog("New UI died")
    while true:
        buf = soc.read(512)
        print buf

main()

