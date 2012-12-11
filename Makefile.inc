CC=gcc
CFLAGS=-g
CSOLVE=/home/abakst/csolve/src/csolve

ifeq ($(CSOLVE),)
$(error CSOLVE variable not set in src/Makefile.inc)
endif
