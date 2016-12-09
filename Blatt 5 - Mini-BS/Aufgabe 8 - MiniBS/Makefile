CC=gcc
CFLAGS=-Wall -Werror -pedantic-errors -g --std=c99  -Wextra

OBJS = process_switch.o process_manager.o syscalls.o processes.o

minibs: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

process_switch.o: process_switch.h process_switch.c
process_manager.o: process_manager.c process_manager.h process_switch.h syscalls.h
syscalls.o: syscalls.c syscalls.h process_manager.h process_switch.h
processes.o: processes.c syscalls.h

clean:
	-rm *.o *~

cleanall: clean
	-rm minibs

.PHONY: tags

tags:
	ctags *.[ch]
