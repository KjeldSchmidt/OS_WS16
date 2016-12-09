/* Funktionen fuer Kontextwechsel
 * Diese Datei darf nicht veraendert werden.
 */

#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/time.h>

#include "process_switch.h"


static ucontext_t error_context;

/* Funktion, die aufgerufen wird, wenn ein Prozess aus der Funktion
 * process_wrapper zurueckkehrt */
void error_function()
{
  fprintf(stderr, "Error: error_function reached.\n");
  if (active_process!=NULL)
    fprintf(stderr, "active_process->pid==%d\n", active_process->pid);
  exit(EXIT_FAILURE);
}


/* Initialisiert statische Daten die innerhalb dieses Moduls verwendet werden
 * Muss einmal zu Beginn ausgefuehrt werden bevor andere Funktionen aus dieser
 * Datei ausgefuehrt werden duerfen. */
void init_process_switch()
{
  if (getcontext(&error_context) == -1) {
    fprintf(stderr, "Error: Could not create initial process context\n");
    exit(EXIT_FAILURE);
  }
  error_context.uc_link = NULL;
  error_context.uc_stack.ss_sp = malloc(STACK_SIZE);
  error_context.uc_stack.ss_size = STACK_SIZE;

  makecontext (&error_context, (void (*) (void)) error_function, 0);

}


/* Initialisiert den Kontext der Prozessstruktur p so, dass die Funktion
 * wrapper_process mit p als Parameter aufgerufen wird, so bald Prozess p das
 * erste Mal ausgefuehrt wird. */
void init_process(process_t *p)
{
  if (getcontext(&(p->context)) == -1) {
    fprintf(stderr, "Error: Could not create process context\n");
    exit(EXIT_FAILURE);
  }
  p->context.uc_link = &error_context;
  p->context.uc_stack.ss_sp = malloc(STACK_SIZE);
  p->context.uc_stack.ss_size = STACK_SIZE;

  makecontext (&(p->context), (void (*) (void)) process_wrapper, 1, p);
}

/* Muss aufgerufen werden, wenn p beendet wurde und alle Ressourcen freigegeben
 * werden sollen */
int free_process(process_t *p)
{
  free(p->context.uc_stack.ss_sp);
  return 0;
}


/* Fuehrt den eigentlichen Kontextwechsel durch.
 * Speichert den aktuellen Kontext in oldp und laedt Kontext aus newp.
 * ist oldp NULL wird nur newp geladen. */
void switch_process(process_t *oldp, process_t *newp)
{

  if (oldp == NULL) {
    fprintf(stderr, "Starting with %d\n", newp->pid);
    setcontext(&(newp->context));
  } else {
    fprintf(stderr, "Switching from %d to %d\n", oldp->pid, newp->pid);
    swapcontext(&(oldp->context), &(newp->context));
  }
}
