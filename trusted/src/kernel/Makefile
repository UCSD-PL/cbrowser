include ../../../Makefile.inc
TRUSTED = $(CURDIR)/../..
CFLAGS += -O0 -I /home/abakst/scratch/csolve/lib -I $(TRUSTED)/inc
CFLAGS += -I $(CURDIR)/../../../libs/libsoup-2.34.0/ 
CFLAGS += $(shell pkg-config gio-2.0 --cflags)
CFLAGS += -DGLIB_VERSION_MIN_REQUIRED=GLIB_VERSION_2_26
LFLAGS += -L$(CURDIR)/../../../trusted/src/kernel -ltrusted
LFLAGS += -L$(CURDIR)/../../../trusted/src/msg    -lmsg

OBJS= tags.o \
      cookie_util.o

all: libtrusted.so

libtrusted.so: $(OBJS)
	gcc -shared -o libtrusted.so $(OBJS)	

%.o: %.c
	$(CC) -c -fPIC $(CFLAGS) $< -o $@

clean: clean_src clean_cookies clean_csolve

clean_src:
	-rm -f *.o *~ *.so

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
