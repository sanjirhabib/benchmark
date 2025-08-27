#run debug with sanitizer
#CFLAGS=-g -lm -fPIC -fvisibility=hidden -fsanitize=address -lasan

#ffi debug
#CFLAGS=-g -lm -fPIC -fvisibility=hidden

#release
CFLAGS=-g -O2 -lm -fPIC -fvisibility=hidden -Iinclude -I.

CLIB=-L.

SRC=$(shell cat source.list | sed 's/.c$$//') 
OBJ=$(addprefix obj/,$(SRC:=.o))
CSRC=$(addprefix ,$(SRC:=.c))
INC=$(addprefix include/,$(SRC:=.h))


libcvar.a: $(INC) $(OBJ)
	ar rcs $@ $(OBJ)
	make.api.h > cvar.h

clean:
	rm -f libcvar.a
	rm -f cvar.h
	rm -f obj/*.o
	rm -f include/*.h

include/%.h: %.c
	cheader $< include

obj/%.o: %.c
	gcc -c -o $@ $< $(CFLAGS)
