#define _POSIX_C_SOURCE 200809L

#ifndef __syscalls_h
#define __syscalls_h

void enterKernelBlockSignals();
void exitKernelUnblockSignals();

/* Erzeugt neuen Prozess mit Hauptfunktion function */
int create_process(void (*function)(int));

/* Wartet auf Beendigung von Prozess pid und gibt die mit pid verbundenen
 * Ressourcen frei.
 * Wenn Prozess pid schon beendet wurde kehrt die Funktion gleich zurueck.  Wenn
 * Prozess pid noch nicht beendet ist blockiert die Funktion bis zu dessen Ende.
 * Es wird nicht ueberprueft, ob pid existiert (ist pid ungueltig, blockiert die
 * Funktion einfach ohne aufgeweckt zu werden) */
void waitpid(int pid);

/* gibt "process <pid>: <string>" auf dem Bildschirm aus */
void println(char *string);


/* signal/wait */

/* neue Signalvariable erzeugen, bzw. nicht mehr benoetigte freigeben */
int mbs_signal_new();
int mbs_signal_free(int sid);

/* swait/ssignal - siehe VL, Kap. 4 */
int mbs_swait(int sid);
int mbs_signal(int sid);



#endif
