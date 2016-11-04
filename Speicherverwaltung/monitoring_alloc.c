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
int totalAllocatedSize = 0;
int allocatedSize = 0;

// Schreiben Sie hier ihre Implementierung für Aufgabenteil a).

void init_monitoring_alloc() {

}

int shutdown_monitoring_alloc() {
  unsigned long leakingBytes = 0;

  printf("Total Allocated Bytes: %d\n",totalAllocatedSize );

  for ( int i = 0; i < MAX_ALLOCATIONS; i++ ) {
      if ( allocated_blocks[i].frame != NULL ) {
        leakingBytes += allocated_blocks[i].size;
      }
  }

  printf("ERROR: Leaking %lu bytes in total!\n", leakingBytes);
  return leakingBytes;
}

void *monitoring_alloc_malloc(size_t size) {
  void *allocated = NULL;

  printf("Requested %lu bytes\n", size );

  if ( ( allocatedBlocksCounter <= MAX_ALLOCATIONS ) && ( allocatedSize + size < MAX_TOTAL_ALLOCATION_SIZE ) ) {
    AllocatedMemoryBlock* amb_pointer = (AllocatedMemoryBlock*) malloc( sizeof( AllocatedMemoryBlock ) );
    amb_pointer->size = size;
    amb_pointer->frame = malloc( size );
    amb_pointer->ordinal = allocatedBlocksCounter;

    allocated_blocks[ allocatedBlocksCounter ] = *amb_pointer;

    allocatedBlocksCounter++;
    allocatedSize += size;
    totalAllocatedSize += size;
    if ( amb_pointer->frame != NULL ) {
      allocated = amb_pointer->frame;
    }
  }

  if(!allocated) {
    printf("ERROR: Block of size %lu could not be allocated!\n",
        (unsigned long)size);
  }
  return allocated;
}


void monitoring_alloc_free(void *ptr) {
  int found = 0;

  AllocatedMemoryBlock* amb_pointer = NULL;
  for ( int i = 0; i < MAX_ALLOCATIONS; i++ ) {
      if ( allocated_blocks[i].frame == ptr ) {
        found = 1;
        amb_pointer = &allocated_blocks[i];
      }
  }

  allocatedSize -= amb_pointer->size;

  free( amb_pointer->frame );
  amb_pointer->frame = NULL;
  amb_pointer->size = 0;

  if(!found) {
    printf("ERROR: Block %p not allocated!\n", ptr);
  }
}
