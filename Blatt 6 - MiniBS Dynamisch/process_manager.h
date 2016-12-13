#ifndef __PROCESS_MANAGER_H
#define __PROCESS_MANAGER_H

#include <ucontext.h>

/* 
 *
 * Datentypen und globale Variablen
 *
 */

/* Konstanten fuer den Prozesszustand */
typedef enum proc_state {
  STATE_RUNNABLE, /* laufende und lauffaehige Prozesse */
  STATE_BLOCKED,  /* wartende Prozesse */
  STATE_ZOMBIE    /* beendete Prozesse */
} proc_state_t;

/* Prozessstruktur, enthaelt alle Daten eines Prozesses */
typedef struct process {
  int pid;               /* Eindeutige Prozessnummer */
  proc_state_t state;    /* Zustand des Prozesses */
  void (*function)(int); /* Hauptfunktion die ausgefuehrt werden soll */
  struct process *next;  /* verwendet fuer Warteschlangen */
  int waiting_for;       /* id des Prozesses auf den dieser Proz. wartet */
  ucontext_t context;    /* Kontext fuer process_switch.c */
} process_t;

/* Listenverwaltung (einfach verkettet) */
typedef struct pqueue {
  process_t *first;  /* Beginn der Liste */
  process_t *last;   /* Ende der Liste */
} pqueue_t;

/* der momentan laufende Prozess */
extern process_t *active_process;

extern pqueue_t run_queue;
extern pqueue_t wait_queue;
extern pqueue_t zombie_queue;


/*
 *
 * Funktionen
 *
 */

/* Prozesse */

/* Hauptfunktion fuer ersten Prozess der aufgerufen wird.
 * muss in processes.c implementiert werden */
extern void init(int pid);

/* Wird aufgerufen wenn proc das erste Mal ausgefuehrt wird.
 * Ruft die Hauptfunktion des Prozesses auf (proc->function).
 * Raeumt nach Ende der Hauptfunktion auf (macht den Prozess zum Zombie) */
void process_wrapper(process_t *proc);

/* Schaltet auf den naechsten Prozess um und ruft switch_process in
 * process_switch.c auf */
void schedule();

/* waehlt den naechsten auszufuehrenden Prozess aus */
process_t *select_next_process();


/* Initialisierung, muss in main aufgerufen werden
 * definiert in syscalls.c */
void init_mbs_signal_module();


/* Warteschlangen */

/* Anfuegen von p an das Ende von queue */
void queue_append(pqueue_t *queue, process_t *p);

/* Ersten Eintrag aus queue entfernen und zurueckliefern */
process_t *queue_get_head(pqueue_t *queue);

/* Eintrag mit pid in queue suchen, austragen und zurueckliefern
 * Liefert NULL falls pid nicht in queue */
process_t *queue_get_process(pqueue_t *queue, int pid);

/* Eintrag suchen der auf waitpid wartet, austragen und zurueckliefern
 * Liefert NULL falls pid nicht in queue */
process_t *queue_get_process_waiting_for(pqueue_t *queue, int waitpid);

/* process aus queue entfernen 
 * liefert -1 falls process nicht in queue, sonst 0 */
int queue_dequeue(pqueue_t *queue, process_t *proc);

#endif
