#define _POSIX_C_SOURCE 200809L

#ifndef __process_switch_h
#define __process_switch_h

#include "process_manager.h"

#define STACK_SIZE 8*1024


/* Initialisiert statische Daten die innerhalb dieses Moduls verwendet werden
 * Muss einmal zu Beginn ausgefuehrt werden bevor andere Funktionen aus dieser
 * Datei ausgefuehrt werden duerfen. */
void init_process_switch();


/* Initialisiert den Kontext der Prozessstruktur p so, dass die Funktion
 * wrapper_process mit p als Parameter aufgerufen wird, so bald Prozess p das
 * erste Mal ausgefuehrt wird. */
void init_process(process_t *p);


/* Muss aufgerufen werden, wenn p beendet wurde und alle Ressourcen freigegeben
 * werden sollen */
int free_process(process_t *p);


/* Fuehrt den eigentlichen Kontextwechsel durch.
 * Speichert den aktuellen Kontext in oldp und laedt Kontext aus newp.
 * ist oldp NULL wird nur newp geladen. */
void switch_process(process_t *oldp, process_t *newp);

#endif
