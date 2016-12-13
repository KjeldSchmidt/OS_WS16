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

char possiblyReplace( char cur ) {
  if ( cur == '1' ) {
    changeOutputColor( 1 );
    return '@';
  }
  if ( cur == '2' ) {
    changeOutputColor( 2 );
    return '@';
  }
  if ( cur == '3' ) {
    changeOutputColor( 3 );
    return '@';
  }
  if ( cur == '4' ) {
    changeOutputColor( 4 );
    return '@';
  }
  if ( cur == '5' ) {
    changeOutputColor( 5 );
    return '@';
  }
  if ( cur == '6' ) {
    changeOutputColor( 6 );
    return '@';
  }
  if ( cur == '7' ) {
    changeOutputColor( 7 );
    return '@';
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

void printTree() {
  char *cur = tree;
  char print;
  print = possiblyReplace( *cur );
  if ( print != '@' ) {
    printf("%c", possiblyReplace( *cur ) );
  }
  do {
    cur++;
    print = possiblyReplace( *cur );
    if ( print != '@' ) {
      printf("%c", possiblyReplace( *cur ) );
    }
  } while ( *cur != '\n' );
}


void loadTree() {
  FILE *file = fopen( "tree.txt", "r" );
  while ( fgets( tree, 100000, file ) != NULL ) {
    printTree();
  }
}
