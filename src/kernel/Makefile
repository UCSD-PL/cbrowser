include ../Makefile.inc
CFLAGS += -O0

OBJS=kernel.o \
	str.o \
	proc.o \
	opt.o \
	msg.o \
	cookie.o \
	ownership.o

all : kernel

kernel: $(OBJS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	-rm -f *.o *~ kernel
