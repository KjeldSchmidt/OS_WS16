CC=gcc
CFLAGS=-Wall -Werror -pedantic-errors -ggdb -std=c99

OBJS = mycp.o

mycp: $(OBJS)
	$(CC) $(CFLAGS) $(CLIBS) -o $@ $(OBJS)

mycp.o: mycp.c mycp.h
	$(CC) $(CFLAGS) $(CLIBS) -c mycp.c

clean:
	-rm *.o *~ tree

.PHONY: tags

tags:
	ctags *.[ch]
