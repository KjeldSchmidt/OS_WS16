#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <limits.h>

#include "process_manager.h"
#include "process_switch.h"
#include "syscalls.h"

pqueue_t run_queue;
pqueue_t wait_queue;
pqueue_t zombie_queue;

process_t *active_process;


/* Hauptfunktion dient zum initialisieren und starten des ersten Prozesses */
int main(int argc, char **argv) 
{
  init_process_switch(); /* Prozessumschaltung initialisieren */

  init_mbs_signal_module(); /* Signale initialisieren */

  active_process = NULL;

  create_process(init); /* Ersten Prozess erzeugen */

  schedule(); /* mit schedule zum ersten Prozess umschalten */

  /* create_process erzeugt einen Prozess, danach wird schedule
   * aufgerufen. Da das der einzige Prozess ist, wird er
   * ausgewaehlt, somit sollte schedule nicht zurueckkehren! */
  fprintf(stderr, "Error: main returned from create_process()!\n");
  return 1;
}







/* Wird aufgerufen wenn Proz. 'proc' das erste Mal ausgefuehrt wird.
 * Ruft die Hauptfunktion des Prozesses auf (proc->function).
 * Raeumt nach Ende der Hauptfunktion auf (macht den Prozess zum Zombie) */
void process_wrapper(process_t *proc) {
  process_t *waiting_proc = NULL;
  proc->function(proc->pid); /* Hauptfunktion des Prozesses aufrufen */

  /* Hauptfunktion beendet -> Prozess beenden */
  fprintf(stderr, "Process %d finished\n", proc->pid);
  proc->state = STATE_ZOMBIE;

  waiting_proc = queue_get_process_waiting_for(&wait_queue, proc->pid);
  if (waiting_proc != NULL) {
    waiting_proc->state = STATE_RUNNABLE;
    queue_append(&run_queue, waiting_proc);
  }
  schedule();
  /* Ein zombie Prozess darf nicht ausgefuehrt werden -> schedule
   * sollte nicht in diesen Prozess zurueckkehren! */
  fprintf(stderr, "Error: schedule returned in \
      finalize_process, process %d\n", proc->pid);
  exit(EXIT_FAILURE);
}


/* Sucht den naechsten auszufuehrenden Prozess aus und ruft switch_process in
 * process_switch.c auf */
void schedule() 
{
  process_t *old_proc = NULL; /* bisheriger Prozess */
  /* zu Beginn, wenn noch kein Prozess laeuft ist active_process==NULL */
  if (active_process != NULL) { 
    /* active_process in die richtige queue einhaengen */
    switch(active_process->state) {
      case STATE_RUNNABLE:
        queue_append(&run_queue, active_process);
        break;
      case STATE_BLOCKED:
        queue_append(&wait_queue, active_process);
        break;
      case STATE_ZOMBIE:
        queue_append(&zombie_queue, active_process);
        break;
    }
  }
  /* bisherigen Prozess merken und neuen auswaehlen */
  old_proc = active_process;
  active_process = select_next_process();

  if (active_process == NULL) {
    /* Unix-Prozess ggf. beenden wenn keine Prozesse mehr lauffaehig sind */
    fprintf(stderr, "No more active processes -> terminating\n");
    exit(EXIT_SUCCESS);
  }

  /* Kontextwechsel veranlassen */
  if (active_process != old_proc) {
    switch_process(old_proc, active_process);
  }
}

/* naechsten auszufuehrenden Prozess auswaehlen 
 * -- waehlt den lauffaehigen Prozess mit der kleinsten PID aus */
process_t *select_next_process()
{
  process_t *proc;
  int smallest_pid = INT_MAX; /* INT_MAX = groesster Wert in einem int */

  if (run_queue.first == NULL)
    return NULL;

  for ( proc = run_queue.first; proc != NULL; proc = proc->next) {
    if (proc->pid < smallest_pid) {
      smallest_pid = proc->pid;
    }
  }
  return queue_get_process(&run_queue, smallest_pid);
}

/*
 *
 * Warteschlangenverwaltung
 *
 */

/* Anfuegen von p an das Ende von queue */
void queue_append(pqueue_t *queue, process_t *p) 
{
  p->next = NULL;
  if (queue->first == NULL) { /* queue is empty */
    queue->first = p;
    queue->last = p;
  } else {
    /* queue not empty */
    queue->last->next = p;
    queue->last = p;
  }
  return;
}

/* Ersten Eintrag aus queue entfernen und zurueckliefern */
process_t *queue_get_head(pqueue_t *queue) 
{
  process_t *result = queue->first;

  if (queue->first != NULL) {
    queue->first = queue->first->next;
    result->next = NULL;
    if (queue->first == NULL)
      queue->last = NULL;
  }
  return result;
}

/* Eintrag mit pid in queue suchen, austragen und zurueckliefern
 * Liefert NULL falls pid nicht in queue */
process_t *queue_get_process(pqueue_t *queue, int pid) 
{
  process_t *proc;

  for ( proc = queue->first; proc != NULL; proc = proc->next) {
    if (proc->pid == pid) {
      queue_dequeue(queue, proc);
      return proc;
    }
  }
  return NULL;
}

/* Eintrag suchen der auf waitpid wartet, austragen und zurueckliefern
 * Liefert NULL falls pid nicht in queue */
process_t *queue_get_process_waiting_for(pqueue_t *queue, int waitpid) 
{
  process_t *proc;

  for (proc = queue->first; proc != NULL; proc = proc->next) {
    if (proc->waiting_for == waitpid) {
      queue_dequeue(queue, proc);
      return proc;
    }
  }
  return NULL;
}

/* process aus queue entfernen 
 * liefert -1 falls process nicht in queue, sonst 0 */
int queue_dequeue(pqueue_t *queue, process_t *process) 
{
  process_t *result = queue->first;
  process_t *previous = NULL;

  while(result != NULL) {
    if (result == process) {
      if (previous == NULL) {
        queue->first = result->next;
      } else {
        previous->next = result->next;
      }
      if (queue->last == result)
        queue->last = previous;
      result->next = NULL;
      return 0;
    } 

    previous = result;
    result = result->next;
  }
  return -1;
}



