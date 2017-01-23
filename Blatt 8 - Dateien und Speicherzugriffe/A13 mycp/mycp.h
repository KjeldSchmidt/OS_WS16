#define _POSIX_C_SOURCE 200809L

#ifndef mycp_header
#define mycp_header

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

void readArgs();
int openSource( char *path );
int createTarget( char *path );
void getSourceLength( int fileDescriptor );
void setTargetSize( off_t targetSize );
void *mapFileToMemory( int fileDescriptor, int flag );
void copyFilesInMemory( const void *source, void *target );


#endif
