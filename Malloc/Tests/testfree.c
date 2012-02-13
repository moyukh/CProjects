#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

int
main( int argc, char **argv )
{
  int* n = (int*)0;
  free(n);

  n = (int*)malloc(sizeof(int)*8);
  int i;
  for (i = 0; i < 8; i++)
    n[i] = i;
  
  free(n + 2);
  
  for (i = 0; i < 8; i++)
    assert(n[i] == i);
  
  free((char*)0x895F6C5A);

  return 0;
}
