#define _POSIX_C_SOURCE 200809L
#include "zeitmessung.h"

/*
 * Aufgabenteil a) Die Seitengröße des Systems beträgt 4096 bytes
 * Aufgabenteil b) Die Zeitauflösung beträgt 1ns (wird auch ausgegeben)
 *
 *
 */

struct timespec resolution;
struct timespec no_pagefaults_begin_time;
struct timespec no_pagefaults_end_time;
struct timespec pagefaults_begin_time;
struct timespec pagefaults_end_time;

char data[2097152];

int main()  {

  clock_getres(CLOCK_PROCESS_CPUTIME_ID, &resolution );
  printf( "Resolution in Nanoseconds: %ld\n", resolution.tv_nsec );

  noPagefaultsTest();
  PagefaultsTest();

  printResults();
  return 0;
}

void noPagefaultsTest() {
  char reader;
  //load first page before measuring time
  reader = data[0];
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &no_pagefaults_begin_time);
  for ( int i = 0; i < 511; ++i ) {
    reader = data[i + 1];
  }
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &no_pagefaults_end_time);
  data[0] = reader;
}

void PagefaultsTest() {
  char reader;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &pagefaults_begin_time);
  for ( int i = 0; i < 511; ++i ) {
    reader = data[ i * 4096 + 1];
  }
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &pagefaults_end_time);
  data[0] = reader;
}

void printResults() {
  //Print time needed without pagefaults
  int secondsPassed = no_pagefaults_end_time.tv_sec - no_pagefaults_begin_time.tv_sec;
  int nSecondsPassed = no_pagefaults_end_time.tv_nsec - no_pagefaults_begin_time.tv_nsec;
  long nSecsPassed = secondsPassed * 1000000000 + nSecondsPassed;
  int averageAccessTime = nSecsPassed / 512;

  printf( "Time passed without pagefaults: %d seconds and %d Nanoseconds\n", secondsPassed, nSecondsPassed);
  printf( "Average read time was %dns\n", averageAccessTime );

  //Print time needed with pagefaults
  secondsPassed = pagefaults_end_time.tv_sec - pagefaults_begin_time.tv_sec;
  nSecondsPassed = pagefaults_end_time.tv_nsec - pagefaults_begin_time.tv_nsec;
  nSecsPassed = secondsPassed * 1000000000 + nSecondsPassed;
  averageAccessTime = nSecsPassed / 512;

  printf( "Time passed with pagefaults: %d seconds and %d Nanoseconds\n", secondsPassed, nSecondsPassed);
  printf( "Average read time was %dns\n", averageAccessTime );

}
