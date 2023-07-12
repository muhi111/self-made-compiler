CFLAGS=-std=c11 -g -static

smc: self_made_compiler.c
		gcc self_made_compiler.c -o smc

test: smc
		./test.sh

clean:
		rm -f smc *.o tmp*

.PHONY: test clean