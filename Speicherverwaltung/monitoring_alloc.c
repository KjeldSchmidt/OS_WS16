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
AllocatedMemoryBlock* allocated_blocks[MAX_ALLOCATIONS];
int allocatedBlocksCounter = 0;
int allocatedSize = 0;

// Schreiben Sie hier ihre Implementierung für Aufgabenteil a).

void init_monitoring_alloc() {

}

int shutdown_monitoring_alloc() {
  unsigned long leakingBytes = 0;

  for ( int i = 0; i < MAX_ALLOCATIONS; i++ ) {
    if ( allocated_blocks[i] != NULL ) {
      if ( (*allocated_blocks[i]).frame != NULL ) {
        leakingBytes += (*allocated_blocks[i]).size;
      }
    }
  }

  printf("ERROR: Leaking %lu bytes in total!\n", leakingBytes);
  return leakingBytes;
}

void *monitoring_alloc_malloc(size_t size) {
  void *allocated = NULL;

  if ( ( allocatedBlocksCounter < MAX_ALLOCATIONS ) && ( allocatedSize + size < MAX_TOTAL_ALLOCATION_SIZE ) ) {
    AllocatedMemoryBlock* amb_pointer = (AllocatedMemoryBlock*) malloc( sizeof( AllocatedMemoryBlock ) );
    amb_pointer->size = size;
    amb_pointer->frame = malloc( size );
    amb_pointer->ordinal = allocatedBlocksCounter;

    allocated_blocks[ allocatedBlocksCounter ] = amb_pointer;

    allocatedBlocksCounter++;
    allocatedSize += size;
    if ( amb_pointer->frame != NULL ) {
      allocated = amb_pointer->frame;
    }
  }

  if( !allocated ) {
    printf( "ERROR: Block of size %lu could not be allocated!\n", (unsigned long) size );
  }
  return allocated;
}


void monitoring_alloc_free(void *ptr) {
  int found = 0;
  int indexFound = 0;

  AllocatedMemoryBlock* amb_pointer = NULL;
  for ( int i = 0; i < MAX_ALLOCATIONS; i++ ) {
    if ( allocated_blocks[i] != NULL ) {
      if ( (*allocated_blocks[i]).frame == ptr ) {
        found = 1;
        indexFound = i;
        amb_pointer = allocated_blocks[i];
      }
    }
  }

  if ( found ) {
      allocatedSize -= amb_pointer->size;
      free( amb_pointer->frame );
      amb_pointer->frame = NULL;
      free( allocated_blocks[ indexFound ] );
      allocated_blocks[ indexFound ] = NULL;
  } else {
    printf("ERROR: Block %p not allocated!\n", ptr);
  }
}
