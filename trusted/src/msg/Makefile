include ../../../Makefile.inc
TRUSTED = $(CURDIR)/../..
CFLAGS += -O0 -I/home/abakst/scratch/csolve/lib -I$(TRUSTED)/inc

OBJS= msg.o

all: libmsg.so

libmsg.so: $(OBJS)
	gcc -shared -o libmsg.so $(OBJS)

%.o: %.c
	gcc -c -fPIC $(CFLAGS) $< -o $@

clean:
	-rm *.o *.so
