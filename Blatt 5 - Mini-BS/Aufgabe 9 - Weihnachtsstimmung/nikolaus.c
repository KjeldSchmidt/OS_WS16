#include "nikolaus.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char tree[100000];
int treeLength = 0;

char *replace(const char *s, char ch, const char *repl) {
    int count = 0;
    const char *t;
    for(t=s; *t; t++)
        count += (*t == ch);

    size_t rlen = strlen(repl);
    char *res = malloc(strlen(s) + (rlen-1)*count + 1);
    char *ptr = res;
    for(t=s; *t; t++) {
        if(*t == ch) {
            memcpy(ptr, repl, rlen);
            ptr += rlen;
        } else {
            *ptr++ = *t;
        }
    }
    *ptr = 0;
    return res;
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

int main() {
  loadTree();
}
