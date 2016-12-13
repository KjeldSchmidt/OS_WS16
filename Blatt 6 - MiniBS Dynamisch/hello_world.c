#include "syscalls.h"

#define _POSIX_C_SOURCE 200809L

void start(int pid) {
  println("Hello world (from a dynamically loaded process)!");
}
