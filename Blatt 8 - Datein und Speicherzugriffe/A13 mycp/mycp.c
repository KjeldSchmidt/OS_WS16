#include "mycp.h"

int source;
int target;

void main( int argc, char* argv[] ) {

  //initialize?

  //main stuff:

  openSource( char *path[] );
  // handle: file does not exist
  createTarget( char *path[] );
  // handle: file already exists;
  // handle: file could not be created;
  getSourceLength( int fileDescriptor );
  // handle: generic error
  setTargetSize( size_t targetSize );
  // handle: generic error
  mapFileToMemory( int fileDescriptor );
  // handle: generic error
  copyFilesInMemory( int source, int target );
  // handle: generic error

  //cleanup:
    //munmap both files
    //close both files
}

int openSource( char *path[] ) {

}

int createTarget( char *path[] ) {

}

int getSourceLength( size_t fileDescriptor ) {

}

int setTargetSize( size_t targetSize ) {

}

int mapFileToMemory( size_t fileDescriptor ) {

}

int copyFilesInMemory( size_t source, size_t target ) {

}
