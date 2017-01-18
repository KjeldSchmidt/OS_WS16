#ifndef mycp_header
#define mycp_header

int openSource( char *path[] );
int createTarget( char *path[] );
int getSourceLength( int fileDescriptor );
int setTargetSize( size_t targetSize );
int mapFileToMemory( int fileDescriptor );
int copyFilesInMemory( int source, int target );


#endif
