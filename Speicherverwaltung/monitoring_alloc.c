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
AllocatedMemoryBlock* allocated_blocks[MAX_ALLOCATIONS];	// An array of POINTERS to Allocated Memoy Blocks.
int allocatedBlocksCounter = 0;								// Counts how many blocks are initialized, in order to throw an error when MAX_ALLOCATIONS is reached.
int allocatedSize = 0;										// Checks the total size of allocated space, n order to throw an error when MAX_TOTAL_ALLOCATION_SIZE is reached.



/*
 * We probably should have done something here, since it was predefined,
 * Like do some magic to keep allocated_blocks an array of AllocatedMemoryBlock's, rather than pointers
 * Which might have made it possible to complete the task without naming the struct.
 */
void init_monitoring_alloc() {
}

/*
 *	This function is called whe the user program exists
 *	And informs about any number of bytes that have not been cleared up.
 */
int shutdown_monitoring_alloc() {
  unsigned long leakingBytes = 0;

  for ( int i = 0; i < MAX_ALLOCATIONS; i++ ) {			// Check all entries...
    if ( allocated_blocks[i] != NULL ) {				// If they aren't nullpointers, anyway.
      if ( (*allocated_blocks[i]).frame != NULL ) {		// If the AMB's frame is not NULL, there is leakage! (Notation is quirky - we're dereferencing the pointer in the array, which gives us an actual AMB, whose frame we can check.)
        leakingBytes += (*allocated_blocks[i]).size;	// And the size tells us how many bytes.
      }
    }
  }

  printf("ERROR: Leaking %lu bytes in total!\n", leakingBytes);
  return leakingBytes;
}


/*
 *	Used to allocate memory with monitoring.
 *	This helps to find memory leaks.
 *	Argument Size: The Size of memory to be allocated, in bytes.
 *  Return: A pointer to the allocated memory.
 */
void *monitoring_alloc_malloc(size_t size) {
  void *allocated = NULL;

  if ( ( allocatedBlocksCounter < MAX_ALLOCATIONS ) && ( allocatedSize + size < MAX_TOTAL_ALLOCATION_SIZE ) ) {	// If our hardcoded constraints are met

    AllocatedMemoryBlock* amb_pointer = (AllocatedMemoryBlock*) malloc( sizeof( AllocatedMemoryBlock ) );		// Try to allocate an AMB.

    if ( amb_pointer != NULL ) {																				// This might already fail, so check for NULL,
      amb_pointer->size = size;																					// Set the size. This is only needed later on, when we check for leaks.
      amb_pointer->frame = malloc( size );																		// Attempt to allocate the requested memory.
      amb_pointer->ordinal = allocatedBlocksCounter;															// Set the ordinal. We never use this, but it's part of the struct declaration.

      allocated_blocks[ allocatedBlocksCounter ] = amb_pointer;													// Insert the AMB-pointer into the apropriate array.
      allocatedBlocksCounter++;																					// Increment the number of allocations done so far.
      allocatedSize += size;																					// And add to the number of currently allocated space, since we have to observe a maximum.

      allocated = amb_pointer->frame;																			// Set the pointer-to-be-returned to the AMB's frame. This is required so the user can use the
    }																											// returned pointer directly, and not mess with our AMB.
  }

  if( !allocated ) {
    printf( "ERROR: Block of size %lu could not be allocated!\n", (unsigned long) size );						// Warn if allocation was not successfull. At this point, we should also free the AMB!
  }
  return allocated;																								// Return the pointer to the frame.
}

/*
 * Used to free memory that has been allocated with monitoring_alloc_malloc.
 * If it was free'd in consuming code directly, the AMB itself would linger in memory.
 * Argument *ptr: A pointer to an AMB's frame. We have to find the corresponding AMB and free it entirely.
 */
void monitoring_alloc_free(void *ptr) {
  int found = 0;										// Have we found the AMB?
  int indexToBeDeleted = 0;								// The index of allocated_blocks where we have found the AMB.

  AllocatedMemoryBlock* amb_pointer = NULL;				// A pointer to an AMB, which will contain the AMB belonging to the frame.
  for ( int i = 0; i < MAX_ALLOCATIONS; i++ ) {			// Check all allocated_blocks...
    if ( allocated_blocks[i] != NULL ) {				// if they aren't NULL.
      if ( (*allocated_blocks[i]).frame == ptr ) {		// Dereference the pointer saved in the array, which gives an AMB. Compare it's frame to ptr.
        found = 1;										// Found the correct AMB.
        indexToBeDeleted = i;							// This is the array index for it.
        amb_pointer = allocated_blocks[i];				// And here we get a pointer to it.
      }
    }
  }

  if ( found ) {										// If we've found the AMB
      allocatedSize -= amb_pointer->size;				// First, subtract the size from the total allocated size, so we may reuse it.
      free( amb_pointer->frame );						// Free the frame first! If we started with the AMB, we wouldn't even have the access to the frame anymore.
      amb_pointer->frame = NULL;						// Set the pointer to NULL. That's just common courtesey.
      free( allocated_blocks[ indexToBeDeleted ] );		// Now we can free the pointer in the aeeay
      allocated_blocks[ indexToBeDeleted ] = NULL;		// And set it to NULL. I believe it's important to free the array index, rather than amb_pointer,
      													// because amb_pointer is in local scope and anything we do to it will be meaningless at the end of this function.

  } else {												// If we haven't found an AMB with the frame, however, we can't delete anything,
    printf("ERROR: Block %p not allocated!\n", ptr);	// And must inform the user of this sad fact. A proper implementation of this would most likely return a status code instead of printing.
  }
}
