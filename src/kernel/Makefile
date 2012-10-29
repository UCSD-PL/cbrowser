include ../Makefile.inc
CFLAGS += -O0 -I /home/abakst/csolve/lib

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

csolve_clean: clean
	-rm -f *.c.ssa.c
	-rm -f *.in.fq
	-rm -f *.out.fq
	-rm -f *.c.json
	-rm -f *.c.tags
	-rm -f *.c.vmap
	-rm -f *.c.csolve.save.c
	-rm -f *.c.annot
	-rm -f *.c.html
	-rm -f *.c.infspec
	-rm -f *.c.log
	-rm -f *.c.out
