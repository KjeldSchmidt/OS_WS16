#include "nikolaus.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define clear() printf("\033[H\033[J")
char tree[1000];

int main() {
  while (1) {
      clear();
      loadTree();
      sleep(1);
  }
}

char replaceColorCharWithNothingAndChangeColorOfTerminalOutput( char cur ) {
  for ( int i = 1; i < 8; ++i ) {
    if ( cur == 48 + i ) {
      changeOutputColor( i );
      return '@';
    }
  }
  if ( cur == '8' ) {
    randomOutputColor();
    return '@';
  }
  return cur;
}

void randomOutputColor() {
  changeOutputColor( ( rand() % 7 ) + 1 );
}

void changeOutputColor( int color ) {
  printf("\33[5;3%dm", color );
}

void printTreeLine() {
  char *cur;
  char print;

  for ( cur = tree; *cur != '\n'; ++cur ) {
    print = replaceColorCharWithNothingAndChangeColorOfTerminalOutput( *cur );
    if ( print != '@' ) {
      printf("%c", replaceColorCharWithNothingAndChangeColorOfTerminalOutput( *cur ) );
    }
  }
  printf("\n");

}


void loadTree() {
  FILE *file = fopen( "tree.txt", "r" );
  while ( fgets( tree, 1000, file ) != NULL ) {
    printTreeLine();
  }
}
