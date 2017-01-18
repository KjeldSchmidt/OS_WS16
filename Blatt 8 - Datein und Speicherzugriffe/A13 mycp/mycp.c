#include "mycp.h"

size_t source;
size_t target;

void main( int argc, char* argv[] ) {

  //initialize

  //main stuff:

  openSource( char *path[] );
  createTarget( char *path[] );
  getSourceLength( size_t fileDescriptor );
  setTargetSize( size_t targetSize );
  mapFileToMemory( size_t fileDescriptor );
  copyFilesInMemory( size_t source, size_t target );

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
