# CC=gcc
# CFLAGS=-g
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

# kernel: kernel.o str.o proc.o msg.o opt.o
# 	${CC} ${CFLAGS} -o kernel kernel.o str.o proc.o msg.o opt.o

# kernel.o: kernel.c kernel.h str.h proc.h
# 	${CC} ${CFLAGS} -c kernel.c

# str.o: str.c str.h
# 	${CC} ${CFLAGS} -c str.c

# proc.o: proc.c proc.h
# 	${CC} ${CFLAGS} -c proc.c

# opt.o: opt.c opt.h
# 	${CC} ${CFLAGS} -c opt.c

# msg.o: msg.c msg.h opt.h str.h
# 	${CC} ${CFLAGS} -c msg.c

# cookie.o: cookie.c cookie.h
# 	${CC} ${CFLAGS} -c cookie.c

# ownership.o: 


clean:
	-rm -f *.o *~ kernel
