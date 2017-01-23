#define _POSIX_C_SOURCE 200809L

#ifndef zeitmessung_header
#define zeitmessung_header

#include <time.h>
#include <stdio.h>

void noPagefaultsTest();
void PagefaultsTest();
void printResults();

#endif
