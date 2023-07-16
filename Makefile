CFLAGS=-std=c11 -g -static
SRCS=main.c codegen.c parse.c
OBJS=$(SRCS:.c=.o)

smc: $(OBJS)
	$(CC) -o smc $(OBJS) $(LDFLAGS)

$(OBJS): smc.h

test: smc
	./test.sh

clean:
	rm -f smc *.o tmp* a.out

.PHONY: test clean