CC=gcc
CFLAGS=-Wall -Werror -pedantic-errors -g --std=c99  -Wextra

nikolaus: nikolaus.o

nikolaus.o: nikolaus.h nikolaus.c

clean:
	-rm *.o *~

cleanall: clean
	-rm minibs

.PHONY: tags

tags:
	ctags *.[ch]
