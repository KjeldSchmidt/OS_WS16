/*
 * monitoring_alloc.c
 *
 * Author: Alexander Ploss <a.ploss@uni-muenster.de>
 *         Michel Steuwer <michel.steuwer@uni-muenster.de>
 *
 */

// Header einbinden.
#include "monitoring_alloc.h"

// Datenstrukturen verwendet von den monitoring_alloc Funktionen.
AllocatedMemoryBlock allocated_blocks[MAX_ALLOCATIONS];
int allocatedBlocksCounter = 0;

// Schreiben Sie hier ihre Implementierung für Aufgabenteil a).

void init_monitoring_alloc() {
  
}

int shutdown_monitoring_alloc() {
  int leakingBytes = 0;

  printf("ERROR: Leaking %lu bytes in total!\n",
      (unsigned long)leakingBytes);
  return leakingBytes;
}


void print_amb( int index, unsigned long size ) {
  AllocatedMemoryBlock amb = allocated_blocks[ index ];
  printf( "Size: %lu\nOrdinal: %ld\n", (unsigned long) size, amb.ordinal );
}

void *monitoring_alloc_malloc(size_t size) {
  void *allocated = NULL;

  if ( allocatedBlocksCounter <= MAX_ALLOCATIONS ) {
    AllocatedMemoryBlock* amb_pointer = malloc( sizeof( AllocatedMemoryBlock ) );
    amb_pointer->size = size;
    amb_pointer->frame = malloc( size );
    amb_pointer->ordinal = allocatedBlocksCounter;

    allocated_blocks[ allocatedBlocksCounter ] = *amb_pointer;

    allocatedBlocksCounter++;
    allocated = amb_pointer->frame;
    
    print_amb( allocatedBlocksCounter, amb_pointer->size );
  }

  if(!allocated) {
    printf("ERROR: Block of size %lu could not be allocated!\n",
        (unsigned long)size);
  }
  return allocated;
}


void monitoring_alloc_free(void *ptr) {
  int found = 0;
  AllocatedMemoryBlock* amb_pointer = (AllocatedMemoryBlock*) ptr;
  free( amb_pointer->frame );
  free( amb_pointer );
  allocatedBlocksCounter--;

  if(!found) {
    printf("ERROR: Block %p not allocated!\n", ptr);
  }
}

