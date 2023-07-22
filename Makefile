CFLAGS=-std=c11 -g -static
SRCS=main.c codegen.c parse.c tokenize.c
OBJS=$(SRCS:.c=.o)

smc: $(OBJS)
	$(CC) -o smc $(OBJS) $(LDFLAGS)

$(OBJS): smc.h

test: smc
	./test.sh

clean:
	rm -f smc *.o *.s tmp* a.out

.PHONY: test clean