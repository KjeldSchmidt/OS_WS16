#include "nikolaus.h"
#include <stdio.h>

int main() {
  changeOutputColor( 3 );
  printf("******\n");
  changeOutputColor( 7 );

}

void changeOutputColor( int color ) {
  printf("\33[5;3%dm", color );
}
