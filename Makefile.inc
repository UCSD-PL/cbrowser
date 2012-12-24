CC=gcc
CFLAGS=-g
CSOLVE=/home/abakst/csolve/src/csolve

TRUSTED=$(CURDIR)/trusted
CHECKED=$(CURDIR)/checked

ifeq ($(CSOLVE),)
$(error CSOLVE variable not set in src/Makefile.inc)
endif
