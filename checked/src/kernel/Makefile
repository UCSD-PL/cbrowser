include ../../../Makefile.inc
CFLAGS += -O0 -I /home/abakst/scratch/csolve/lib -I $(CURDIR)/../../inc -I $(CURDIR)/../../../trusted/inc
#CFLAGS += --manual
LFLAGS += -L$(CURDIR)/../../../trusted/src/kernel -ltrusted
LFLAGS += -L$(CURDIR)/../../../trusted/src/msg    -lmsg

OBJS= main.o \
	util.o \
	kernel.o \
	tab.o 	\
	ui.o \
	wget.o \
	proc.o \
	cookie_hash.o 

all : kernel

kernel: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LFLAGS) -o kernel

verify_%:
	$(CSOLVE) $(CFLAGS) -c --csolveprefix=$(PWD)/verify/$* $*

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean: clean_src clean_cookies clean_csolve

clean_src:
	-rm -f *.o *~ kernel

clean_cookies:
	-rm -rf cookies

clean_csolve:
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
	-rm -rf *.c.autospec
	-rm -rf js
	-rm -rf css
