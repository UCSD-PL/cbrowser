include Makefile.inc

DIRS = kernel
EXE  = kernel

all : $(EXE)

$(EXE) : force_look
	cd kernel; make

.PHONY: cscope

cscope : 
	echo "Building cscope database"
	-rm ./cscope/cscope.files
	find $(PWD) -iname "*.[ch]" > ./cscope/cscope.files
	cd cscope; cscope -b -q

test_kernel: kernel
	cd kernel; ./kernel -l

debug_kernel: kernel
	cd kernel; gdb ./kernel

force_look:
	true

clean :
	-for d in $(DIRS); do (cd $$d; make clean); done

