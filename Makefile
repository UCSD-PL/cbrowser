include Makefile.inc

DIRS = trusted/src/kernel checked/src/kernel
EXE  = kernel

all : $(EXE)

$(EXE) : force_look
	make -C trusted/src/kernel
	make -C checked/src/kernel

.PHONY: cscope 

cscope : 
	echo "Building cscope database"
	-rm ./cscope/cscope.files
	find $(PWD)/ -iname "*.[ch]" > ./cscope/cscope.files
	cd cscope; cscope -b -q

run: kernel
	cd checked/src/kernel; ./kernel -l

debug: kernel
	cd checked/src/kernel; gdb ./kernel

force_look:
	true

clean :
	-for d in $(DIRS); do (cd $$d; make clean); done

