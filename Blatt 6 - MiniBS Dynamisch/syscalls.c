#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>

#include "syscalls.h"
#include "process_manager.h"
#include "process_switch.h"

#define MYPID ((active_process!=NULL)? active_process->pid : -1)

static int next_pid = 0;

/* Erzeugt neuen Prozess mit Hauptfunktion function */
int create_process(void (*function)(int))
{
  int new_pid = next_pid++;
  process_t *new_proc = NULL;

  fprintf(stderr, "process %d called create_process\n", MYPID);
  /* Prozessstruktur anlegen und initialisieren */
  new_proc = (process_t *) malloc (sizeof(process_t));
  init_process(new_proc);
  new_proc->pid = new_pid;
  new_proc->function = function;
  new_proc->state = STATE_RUNNABLE;

  /* in die Warteschlange einfuegen */
  queue_append(&run_queue, new_proc);


  return new_pid;
}

/* Wartet auf Beendigung von Prozess pid und gibt die mit pid verbundenen
 * Ressourcen frei.
 * Wenn Prozess pid schon beendet wurde kehrt die Funktion gleich zurueck.  Wenn
 * Prozess pid noch nicht beendet ist blockiert die Funktion bis zu dessen Ende.
 * Es wird nicht ueberprueft, ob pid existiert (ist pid ungueltig, blockiert die
 * Funktion einfach ohne aufgeweckt zu werden) */
void waitpid(int pid)
{

  process_t *result = NULL;

  fprintf(stderr, "process %d called waitpid\n", MYPID);
  /* nach prozess mit pid pid in der zombie_queue suchen */
  result = queue_get_process(&zombie_queue, pid);
  while (result == NULL) {
    /* warten */
    active_process->state = STATE_BLOCKED;
    active_process->waiting_for = pid;
    schedule();
    /* erneut suchen */
    result = queue_get_process(&zombie_queue, pid);
  }

}

/* gibt "process <pid>: <string>" auf dem Bildschirm aus */
void println(char *string)
{
  fprintf(stderr, "process %d: %s\n", MYPID, string);
}


/*******************************************************************************
 * Signale
 *******************************************************************************/


/* Maximale Anzahl Signalvariablen */
#define NUM_SIGNALS 128

#define CHECK_SID(sid) if (sid < 0 || \
                           sid >= NUM_SIGNALS || \
                           !signals[sid].used) { \
                          return -1; \
                       }
typedef struct {
  char used;
  char value;
  process_t *waiting;
} signal_t;

signal_t signals[NUM_SIGNALS];

/* Funktion zur Initialisierung des Signal Moduls
 */
void init_mbs_signal_module()
{
  int i = 0;

  for (i=0; i < NUM_SIGNALS; i++)
    signals[i].used = 0;
}


/* erzeugt neue Signalvariable (Wert 0) und liefert ihre Nummer zurueck
 * Rueckgabe -1 im Fehlerfall (zu viele Signale vorhanden)
 */
int mbs_signal_new()
{
  int i = 0;


  for (i = 0; i < NUM_SIGNALS; i++) {
    if(!signals[i].used) {
      signals[i].used = 1;
      signals[i].value = 0;
      signals[i].waiting = NULL;
      return i;
    }
  }
  return -1;
}

/* Signalvariable mit ID sid freigeben
 * Rueckgabe -1, falls sid ungueltig, 0 sonst
 */
int mbs_signal_free(int sid)
{
  CHECK_SID(sid);

  signals[sid].used = 0;
  return 0;
}

/* ssignal und swait siehe VL 4.
 * Rueckgabe -1, falls sid ungueltig oder bereits verwendet (immer nur ein
 * Prozess darf warten!), 0 sonst
 */
int mbs_swait(int sid)
{
  CHECK_SID(sid);

  if (signals[sid].waiting != NULL)
    return -1;

  if (signals[sid].value == 0) {
    /* wait for signal */
    active_process->state = STATE_BLOCKED;
    signals[sid].waiting = active_process;
    schedule();
  }

  signals[sid].value = 0;
  signals[sid].waiting = NULL;

  return 0;
}

int mbs_signal(int sid)
{

  CHECK_SID(sid);

  signals[sid].value = 1;
  if(signals[sid].waiting != NULL) {
    /*  Prozess aufwecken */
    signals[sid].waiting->state = STATE_RUNNABLE;
    queue_dequeue(&wait_queue, signals[sid].waiting);
    queue_append(&run_queue, signals[sid].waiting);
  }
  return 0;
}
