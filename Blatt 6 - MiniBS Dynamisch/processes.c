#include "syscalls.h"


static int bed[4]; /* Bedingungsvariablen */

/* Warten mit freigabe */


void proc1(int);
void proc2(int);
void proc3(int);

void proc1(int pid) {
  int i;
  int pid1, pid2;

  /* Initialisierung */
  for (i = 0; i < 4; i++)
    bed[i] = mbs_signal_new();

  println("P1: erzeugt P2");
  pid1 = create_process(proc2);

  println("P1: erzeugt P3");
  pid2 = create_process(proc3);

  println("P1: wartet auf Bed. B1");
  mbs_swait(bed[0]);

  println("P1: wartet auf Bed. B2");
  mbs_swait(bed[1]);

  println("P1: setzt Bed. B3 auf wahr");
  mbs_signal(bed[2]);

  println("P1: setzt Bed. B4 auf wahr");
  mbs_signal(bed[3]);

  println("P1: beendet sich");

  waitpid(pid2);
  waitpid(pid1);
  return;
}

void proc2(int pid) {
  println("P2: setzt Bed. B1 auf wahr");
  mbs_signal(bed[0]);
  
  println("P2: wartet auf Bed. B4");
  mbs_swait(bed[3]);

  println("P2: beendet sich");
  return;
}

void proc3(int pid) {
  println("P3: setzt Bed. B2 auf wahr");
  mbs_signal(bed[1]);

  println("P3: wartet auf Bed. B3");
  mbs_swait(bed[2]);

  println("P3: beendet sich");
  return;
}


/*
 * mit busy-waiting
 */


void bw_proc1(int);
void bw_proc2(int);
void bw_proc3(int);

void bw_proc1(int pid) {
  int i;
  int pid1, pid2;

  /* Initialisierung */
  for (i = 0; i < 4; i++)
    bed[i] = 0;

  println("P1: erzeugt P2");
  pid1 = create_process(bw_proc2);

  println("P1: erzeugt P3");
  pid2 = create_process(bw_proc3);

  println("P1: wartet auf Bed. B1");
  while (!bed[0]) ;

  println("P1: wartet auf Bed. B2");
  while (!bed[1]) ;

  println("P1: setzt Bed. B3 auf wahr");
  bed[2] = 1;

  println("P1: setzt Bed. B4 auf wahr");
  bed[3] = 1;

  println("P1: beendet sich");
  waitpid(pid2);
  waitpid(pid1);
  return;
}

void bw_proc2(int pid) {
  println("P2: setzt Bed. B1 auf wahr");
  bed[0] = 1;
  
  println("P2: wartet auf Bed. B4");
  while (!bed[3]) ;

  println("P2: beendet sich");
  return;
}

void bw_proc3(int pid) {
  println("P3: setzt Bed. B2 auf wahr");
  bed[1] = 1;

  println("P3: wartet auf Bed. B3");
  while(!bed[2]) ;

  println("P3: beendet sich");
  return;
}



void init(int pid) {
  int p;

  println("mbs_signal/mbs_wait");

  p = create_process(proc1);
  waitpid(p);
  println("busy wait");

  p = create_process(bw_proc1);
  waitpid(p);
  println("done");
}

