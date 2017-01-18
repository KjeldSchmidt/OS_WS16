#ifndef mycp_header
#define mycp_header

int openSource( char *path[] );
int createTarget( char *path[] );
int getSourceLength( size_t fileDescriptor );
int setTargetSize( size_t targetSize );
int mapFileToMemory( size_t fileDescriptor );
int copyFilesInMemory( size_t source, size_t target );


#endif
