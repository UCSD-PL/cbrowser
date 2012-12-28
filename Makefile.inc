CC=gcc
CFLAGS=-g
CSOLVE=/home/abakst/scratch/csolve/src/csolve

ifeq ($(CSOLVE),)
$(error CSOLVE variable not set in src/Makefile.inc)
endif
