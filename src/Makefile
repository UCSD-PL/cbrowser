include Makefile.inc

DIRS = kernel
EXE  = kernel

all : $(EXE)

$(EXE) : force_look
	make -C kernel

.PHONY: cscope 

cscope : 
	echo "Building cscope database"
	-rm ./cscope/cscope.files
	find $(PWD) -iname "*.[ch]" > ./cscope/cscope.files
	cd cscope; cscope -b -q

run: kernel
	cd kernel; ./kernel -l

debug: kernel
	cd kernel; gdb ./kernel

force_look:
	true

clean :
	-for d in $(DIRS); do (cd $$d; make clean); done

