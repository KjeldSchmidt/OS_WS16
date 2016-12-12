#include "nikolaus.h"
#include <stdio.h>

int main() {
  changeOutputColor( 3 );
  printf("******\n");

}

void changeOutputColor( ) {
  printf("\33[5;33m" );
}
