#include <stdlib.h>
#include <stdio.h>
#ifndef _WIN32
	#include <unistd.h>
#else
	#include <Windows.h>
#endif

int main(int argc, char *argv[]) {


  if (argc == 1) { // no command line argument is given
    // allocate memory ...
    int i = 1; // ... statically ...
    int *ip = malloc(sizeof(int)); // ... dynamically
    // print addresses
    printf("&i: %lu (hex: %p)\n", (unsigned long)&i, &i);
    printf("ip: %lu (hex: %p)\n", (unsigned long)ip, ip);


		// sleep for 60 seconds
		#ifndef _WIN32
	    sleep(60);
		#else
			Sleep(6000);
		#endif

    // free dynamically allocated memory
		printf("%d\n", i);
		printf("%d\n", *ip);
    free(ip);
    ip = NULL;
  } else if (argc == 2) { // one command line argument is given
    // allocate memory ...
    int i = 1; // ... statically ...
    int *ip = (int*)malloc(sizeof(int)); // ... dynamically
    // print addresses
    printf("&i: %lu (hex: %p)\n", (unsigned long)&i, &i);
    printf("ip: %lu (hex: %p)\n", (unsigned long)ip, ip);

    // convert command line argument to a number
    // and assign to address
    int *address = (int*)strtoul(argv[1], NULL, 0);
    printf("Access memory at address: %p\n", address);
    // access memory at memory location stored in address
    *address = 5;

    // free dynamically allocated memory
    free(ip);
    ip = NULL;
  } else { // more than one command line argument is given
    exit(EXIT_FAILURE);
  }
  printf("done\n");
  exit(EXIT_SUCCESS);
}
