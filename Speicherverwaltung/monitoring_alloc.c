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
AllocatedMemoryBlock allocated_blocks[MAX_ALLOCATIONS]; // An array of AMBs.
int allocatedBlocksCounter = 0;                         // Counts how many AMBs are currently in use, in order to throw an error when MAX_ALLOCATIONS is reached.
int allocatedSize = 0;                                  // Checks the total size of allocated space, in order to throw an error when MAX_TOTAL_ALLOCATION_SIZE is reached.


/*
 * Initialize allocated_blocks
 * By first creating an instance of it, setting default values
 * And then just plain copying it by assignment - since it's not a pointer, this is effectively a copy, unlike in some higher level languages.
 */
void init_monitoring_alloc() {
  AllocatedMemoryBlock AMB;                          // Creating an instance.
  AMB.frame = NULL;                                  // Setting defautl values - very important, since C does not initialize with default values!
  AMB.size = 0;
  AMB.ordinal = 0;
  for ( size_t i = 0; i < MAX_ALLOCATIONS ; ++i ) {
    allocated_blocks[i] = AMB;                       // Copy by assignment.
    allocated_blocks[i].ordinal = i;                 // And set the ordinal, just for fun.
  }
}

/*
 *	This function is called whe the user program exists
 *	And informs about any number of bytes that have not been cleared up.
 */
int shutdown_monitoring_alloc() {
  unsigned long leakingBytes = 0;

  for ( int i = 0; i < MAX_ALLOCATIONS; i++ ) {    // Check all entries...
    if ( allocated_blocks[i].frame != NULL ) {     // Check if they are still pointing somewhere. If so, there is leakage!
      leakingBytes += allocated_blocks[i].size;	 // And the size tells us how many bytes.
    }
  }

  printf("ERROR: Leaking %lu bytes in total!\n", leakingBytes);
  return leakingBytes;                             // Tell the user by how much he failed.
}

/*
 * A small helper function for monitoring_alloc_malloc
 * Which finds an index in allocated_blocks for which the AMB is not in use.
 * We did not declare this function in the header, to avoid changing it (but we should, in a real setting)
 * And it could easily be inside monitoring_alloc_malloc.
 *
 * Return: The index if one is found
 * Return: -1 if not.
 */
int getEmptyAMBIndex() {                          // Find an AMB index that is currently free.
  for ( int i = 0; i < MAX_ALLOCATIONS; ++i ) {   // By checking all entries
    if ( allocated_blocks[i].frame == NULL ) {    // To find the one with a null frame.
      return i;                                   // Return the first one; We don't care.
    }
  }
  return -1;                                      // -1 if there is no free AMB. Note: Should not happen as of the writing of this comment.
}

/*
 *	Used to allocate memory with monitoring.
 *	This helps to find memory leaks.
 *	Argument Size: The Size of memory to be allocated, in bytes.
 *  Return: A pointer to the allocated memory.
 */
void *monitoring_alloc_malloc(size_t size) {
  void *allocated = NULL;

  if (
    allocatedBlocksCounter < MAX_ALLOCATIONS &&
    allocatedSize + size < MAX_TOTAL_ALLOCATION_SIZE )
  {                                                          // Iff our predefined limits are met...

    int ambIndex = getEmptyAMBIndex();                       // Search for an empty AMB.
    if ( ambIndex != -1 ) {																	 // This should currently impossible, because we check allocatedBlocksCounter before - but you never know!
      allocated_blocks[ ambIndex ].size = size;              // Set the size. This is only needed later on, when we check for leaks.
      allocated_blocks[ ambIndex ].frame = malloc( size );   // Attempt to allocate the requested memory.

      if ( allocated_blocks[ ambIndex ].frame != NULL ) {    // If the requested memory could be allocated...
        allocatedBlocksCounter++;                            // Increment the number of currently allocated AMBs.
        allocatedSize += size;                               // And add to the number of currently allocated space, since we have to observe a maximum.
        allocated = allocated_blocks[ ambIndex ].frame;      // Set the pointer-to-be-returned to the AMB's frame. This is required so the user can use the returned pointer directly and not mess with our AMB.
      }
    }
  }

  if( !allocated ) {
    printf(
      "ERROR: Block of size %lu could not be allocated!\n",
      (unsigned long) size
    );                                                       // Warn if allocation was not successful. If it wasn't, no action is neccessary - we set the AMBs size, but it's frame is null, so it doesn't matter anywhere.
  }
  return allocated;                                          // Return the pointer to the frame.
}

/*
 * Used to free memory that has been allocated with monitoring_alloc_malloc.
 * If it was free'd in consuming code directly, we wouldn't know about it - and couldn't reuse it!
 * We would also falsely report leaking bytes.
 *
 * Argument *ptr: A pointer to an AMB's frame. We have to find the corresponding AMB handle the freeing..
 */
void monitoring_alloc_free(void *ptr) {
  int found = 0;                                   // Have we found the AMB corresponding to *ptr?
  int indexToBeDeleted = 0;                        // The index of allocated_blocks where we have found the AMB.

  for ( int i = 0; i < MAX_ALLOCATIONS; i++ ) {    // Check all allocated_blocks...
    if ( allocated_blocks[i].frame == ptr ) {		   // To check it's frame against *ptr.
      found = 1;                                   // Found the correct AMB.
      indexToBeDeleted = i;                        // And it's array index..
    }
  }

  if ( found ) {                                                     // If we've found the AMB
      allocatedSize -= allocated_blocks[ indexToBeDeleted ].size;    // First, subtract the size from the total allocated size, so we may reuse it.
      allocatedBlocksCounter--;                                      // And also decrement the count of used Blocks, so we may reuse them!
      free( allocated_blocks[ indexToBeDeleted ].frame );						 // Free the frame.
      allocated_blocks[ indexToBeDeleted ].frame = NULL;						 // Set the pointer to NULL. This marks it as 'unused'.

  } else {												                                   // If we haven't found an AMB with the frame, however, we can't delete anything,
    printf("ERROR: Block %p not allocated!\n", ptr);                 // And must inform the user of this sad fact. A proper implementation of this would most likely return a status code instead of printing.
  }
}
