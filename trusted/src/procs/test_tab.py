#!/usr/bin/env python

# Quark is Copyright (C) 2012-2015, Quark Team.
#
# You can redistribute and modify it under the terms of the GNU GPL,
# version 2 or later, but it is made available WITHOUT ANY WARRANTY.
#
# For more information about Quark, see our web site at:
# http://goto.ucsd.edu/quark/


import os
import stat
import urllib
import inspect
import time
import string
import cStringIO as StringIO
import cPickle as pickle
import time
import sys
import os
import tempfile
import threading
import urlparse
import msg
import ctypes
import socket
import random
import opt
import re
import struct
import signal
import threading
import select
import shm
import Cookie

domain = ""
urls = ['/', '/bogus.html', '/index.html']
delay = 2

def signal_handler(signal, frame):
    sys.exit(0)

def tlog(str):
    tlog_nonl(str + "\n")

def tlog2(str):
    tlog_nonl(str + "\n")

def tlog_nonl(str):
    sys.stderr.write("T: " + str)
    sys.stderr.flush()

# mostly copied from shm_wrapper.py
def create_memory(size, permissions = 0600):
    """ Creates a new shared memory segment. One can destroy it either
    by calling the module-level method remove_memory() or by calling
    the .remove() method of a handle to said memory.
    """
    memory = None

    while not memory:
        key = random.randint(1, sys.maxint - 1)
        try:
            memory = shm.create_memory(key, size, permissions)
            # for output process
#            memory.setuid(quarkexec.quark_output_uid)
        except shm.error, ExtraData:
            tlog("unexpected error:" + str(sys.exc_info()))
            if shm.memory_haskey(key):
                pass
            else:
                raise shm.error, ExtraData
    return memory

class Tab:
    soc = None
    shm_obj = None

    def __init__(self, s):
        self.soc = s
        self.shm_obj = create_memory(5000000)
        self.shm_obj.attach()

    def test_loop(self):
        while True:
            #Types, hah
            next_url = random.randrange(len(urls))
            next_url = domain + urls[next_url]

            m = msg.create_req_uri_follow(next_url)
            tlog("Writing message: %s" % str(m))
            msg.write_message_soc(m, self.soc)

            m = msg.read_message_soc(self.soc)
            tlog("Received message: %s" % str(m))
            #tlog(m.content)

            time.sleep(5)

            if (random.randrange(10) == 0):
                m = msg.create_display_shm(self.shm_obj.shmid, 0)
                tlog("display_shm(%d, %d)" % (self.shm_obj.shmid, 0))
                tlog("Writing message: %s" % str(m))
                msg.write_message_soc(m, self.soc)

            c = Cookie.SimpleCookie()
            c.load("Set-Cookie: Foo=Bar; Domain="+domain+"; Path=/; HttpOnly")
            m = msg.create_set_cookie(c)
            tlog("Writing message: %s" % str(m))
            msg.write_message_soc(m, self.soc)

            time.sleep(5)


def main():
    global domain
    signal.signal(signal.SIGINT, signal_handler)
    try:
        soc_fd = int(sys.argv[1])
        soc = socket.fromfd(soc_fd, socket.AF_UNIX, socket.SOCK_STREAM)
        domain = sys.argv[2]
        tlog("New tab(%s) listening on socket fd %d" % (sys.argv[2], int(sys.argv[1])))
        tab = Tab(soc)
        tab.test_loop()
    except Exception as e:
        tlog("Tab died: %s" % e)
    tlog("DONE")

libsoup = ctypes.CDLL("../../../libs/libsoup-2.34.0/libsoup/.libs/libsoup-2.4.so")
main()
