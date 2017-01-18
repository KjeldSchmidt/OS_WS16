#define _POSIX_C_SOURCE 200809L
#include "mycp.h"

char *sourcePath;
char *targetPath;
int sourceFD;
int targetFD;
void *sourceMapAddress;
void *targetMapAddress;
struct stat sourceInfo;

void error(char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

int main( int argc, char* argv[] ) {
  if ( argc != 3 ) {
    error( "Incorrect number of arguments given. Please specify source and target." );
  }

  //initialize?
  int success = 0;
  sourcePath = argv[1];
  targetPath = argv[2];


  //main stuff:

  openSource( sourcePath );
  createTarget( targetPath );

  getSourceLength( sourceFD );
  setTargetSize( (off_t) sourceInfo.st_size );

  sourceMapAddress = mapFileToMemory( sourceFD, MAP_PRIVATE );
  targetMapAddress = mapFileToMemory( targetFD, MAP_SHARED );

  copyFilesInMemory( sourceFD, targetFD );

  //cleanup:
  success = munmap( targetMapAddress, sourceInfo.st_size );
  if ( success == -1 ) {
    error( "Unmapping target failed." );
  }

  success = munmap( sourceMapAddress, sourceInfo.st_size );
  if ( success == -1 ) {
    error( "Unmapping source failed." );
  }

  success = close( targetFD );
  if ( success == -1 ) {
    error( "Closing target failed." );
  }

  success = close( sourceFD );
  if ( success == -1 ) {
    error( "Closing source failed." );
  }

  return 0;
}

int openSource( char *path ) {
  int fileDescriptor = 0;
  fileDescriptor = open( path, O_RDONLY );
  if ( fileDescriptor == -1 ) {
    error( "Source file could not be opened." );
  }

  return fileDescriptor;
}

int createTarget( char *path ) {
  int fileDescriptor = 0;
  if (0 == access(path, 0)) {
    error( "Target file already exists. Specify another target and try again." );
  }
  else {
    /*
     *  O_RDWR: This programm can read and write the file.
     *  O_CREAT: This file will be created, if it does not exist.
     *  S_IRWXU: The user who executed this programm will have full control over the newly created file
     *    Which is desireable, since the users copy should be his.
     */
    fileDescriptor = open( path, O_RDWR | O_CREAT, S_IRWXU );
    if ( fileDescriptor < 1 ) {
      error( "Target file could not be created" );
    }
  }

  return fileDescriptor;
}

void getSourceLength( int fileDescriptor ) {
  int success = 0;
  success = fstat( sourceFD, &sourceInfo );

  if ( success == -1 ) {
    error("Getting source length failed.");
  }
}

void setTargetSize( off_t targetSize ) {
  off_t success = 0;
  printf("TargetFD: %d\n", targetFD );
  success = lseek( targetFD, targetSize, SEEK_END );
  if ( success == (off_t) -1 ) {
    error( "Setting target size failed (lseek)" );
  }

  int bytesWritten = 0;
  bytesWritten = write( targetFD, (char) 0, 1);
  if (bytesWritten != 1) {
    error( "Writing to file end failed." );
  }
}

void *mapFileToMemory( int fileDescriptor, int flag ) {
  void *mapAddress = 0;
  mapAddress = mmap( NULL, sourceInfo.st_size, PROT_NONE, flag, fileDescriptor, 0 );
  if ( mapAddress == (void *) -1 ) {
    error( "mapping file failed" );
  }
  return mapAddress;
}

void copyFilesInMemory( int sourceFD, int targetFD ) {
  memcpy( targetMapAddress, sourceMapAddress, sourceInfo.st_size );
  // No check for error code - man page does not explain any errors or faulty return values.
}
