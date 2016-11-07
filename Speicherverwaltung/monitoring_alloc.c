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
AllocatedMemoryBlock allocated_blocks[MAX_ALLOCATIONS];	// An array of POINTERS to Allocated Memoy Blocks.
int allocatedBlocksCounter = 0;								// Counts how many blocks are initialized, in order to throw an error when MAX_ALLOCATIONS is reached.
int allocatedSize = 0;										// Checks the total size of allocated space, n order to throw an error when MAX_TOTAL_ALLOCATION_SIZE is reached.



/*
 * We probably should have done something here, since it was predefined,
 * Like do some magic to keep allocated_blocks an array of AllocatedMemoryBlock's, rather than pointers
 * Which might have made it possible to complete the task without naming the struct.
 */
void init_monitoring_alloc() {
  AllocatedMemoryBlock AMB;
  AMB.frame = NULL;
  AMB.size = 0;
  AMB.ordinal = 0;
  for ( size_t i = 0; i < MAX_ALLOCATIONS ; ++i ) {
    allocated_blocks[i] = AMB;
    allocated_blocks[i].ordinal = i;
  }
}

/*
 *	This function is called whe the user program exists
 *	And informs about any number of bytes that have not been cleared up.
 */
int shutdown_monitoring_alloc() {
  unsigned long leakingBytes = 0;

  for ( int i = 0; i < MAX_ALLOCATIONS; i++ ) {			// Check all entries...
    if ( allocated_blocks[i].frame != NULL ) {				// If they aren't nullpointers, anyway.
      if ( allocated_blocks[i].frame != NULL ) {		// If the AMB's frame is not NULL, there is leakage! (Notation is quirky - we're dereferencing the pointer in the array, which gives us an actual AMB, whose frame we can check.)
        leakingBytes += allocated_blocks[i].size;	// And the size tells us how many bytes.
      }
    }
  }

  printf("ERROR: Leaking %lu bytes in total!\n", leakingBytes);
  return leakingBytes;
}

int getEmptyAMBIndex() {                          // Find an AMB index that is currently free.
  for ( int i = 0; i < MAX_ALLOCATIONS; ++i ) {   // By checking all entries
    if ( allocated_blocks[i].frame == NULL ) {    // To find the one with a null frame.
      return i;                                   // Return the first one; We don't care.
    }
  }
  return -1;                                      // -1 if there is no free AMB. Note: Never happens as of the writing of this comment.
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

    int ambIndex = getEmptyAMBIndex();

    if ( ambIndex != -1 ) {																				// This might already fail, so check for NULL,
      allocated_blocks[ ambIndex ].size = size;																					// Set the size. This is only needed later on, when we check for leaks.
      allocated_blocks[ ambIndex ].frame = malloc( size );																		// Attempt to allocate the requested memory.
      allocated_blocks[ ambIndex ].ordinal = allocatedBlocksCounter;															// Set the ordinal. We never use this, but it's part of the struct declaration.

      //allocated_blocks[ allocatedBlocksCounter ] = amb_pointer;													// Insert the AMB-pointer into the apropriate array.
      allocatedBlocksCounter++;																					// Increment the number of allocations done so far.
      allocatedSize += size;																					// And add to the number of currently allocated space, since we have to observe a maximum.

      allocated = allocated_blocks[ ambIndex ].frame;																			// Set the pointer-to-be-returned to the AMB's frame. This is required so the user can use the
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

  for ( int i = 0; i < MAX_ALLOCATIONS; i++ ) {			// Check all allocated_blocks...
    if ( allocated_blocks[i].frame != NULL ) {				// if they aren't NULL.
      if ( allocated_blocks[i].frame == ptr ) {		// Dereference the pointer saved in the array, which gives an AMB. Compare it's frame to ptr.
        found = 1;										// Found the correct AMB.
        indexToBeDeleted = i;							// This is the array index for it.
      }
    }
  }

  if ( found ) {										// If we've found the AMB
      allocatedSize -= allocated_blocks[ indexToBeDeleted ].size;				// First, subtract the size from the total allocated size, so we may reuse it.
      allocatedBlocksCounter--;
      free( allocated_blocks[ indexToBeDeleted ].frame );						// Free the frame first! If we started with the AMB, we wouldn't even have the access to the frame anymore.
      allocated_blocks[ indexToBeDeleted ].frame = NULL;						// Set the pointer to NULL. That's just common courtesey.

  } else {												// If we haven't found an AMB with the frame, however, we can't delete anything,
    printf("ERROR: Block %p not allocated!\n", ptr);	// And must inform the user of this sad fact. A proper implementation of this would most likely return a status code instead of printing.
  }
}
