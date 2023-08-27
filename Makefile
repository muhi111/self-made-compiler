CFLAGS=-std=c11 -g -static
SRCS=main.c codegen.c parse.c tokenize.c nodegen.c
OBJDIR=./obj
OBJS=$(addprefix $(OBJDIR)/, $(SRCS:.c=.o))

smc: $(OBJS)
	$(CC) -o smc $(OBJS) $(LDFLAGS)

$(OBJDIR)/%.o: %.c
	@[ -d $(OBJDIR) ]
	$(CC) $(CFLAGS) -o $@ -c $<

test: smc
	./test.sh

clean:
	rm -f smc tmp* a.out
	rm -f ./obj/*.o

.PHONY: test clean