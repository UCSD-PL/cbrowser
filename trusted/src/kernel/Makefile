include ../../../Makefile.inc
CFLAGS += -O0 -I /home/abakst/csolve/lib -I $(TRUSTED)/inc

OBJS= msg.o \
        tags.o \
        cookie_util.o

all: libtrusted.a

libtrusted.a: $(OBJS)
	-rm libstrusted.a
	ar -cvq libtrusted.a $(OBJS)

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
	-rm -f *.a
