CC=gcc
CFLAGS=-Wall -Werror -pedantic-errors -ggdb -std=c99

OBJS = zeitmessung.o

zeitmessung: $(OBJS)
	$(CC) $(CFLAGS) $(CLIBS) -o $@ $(OBJS)

zeitmessung.o: zeitmessung.c zeitmessung.h
	$(CC) $(CFLAGS) $(CLIBS) -c zeitmessung.c

clean:
	-rm *.o *~ tree

.PHONY: tags

tags:
	ctags *.[ch]
