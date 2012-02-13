#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

int
main( int argc, char **argv )
{
  int* array = (int*)malloc(100 * sizeof(int));
  int* array2 = (int*)0;
  int* array3 = (int*)0;
  int i;

  for (i = 0; i < 100; i++)
    array[i] = i;
  
  array2 = (int*)realloc(array, 2*100*sizeof(int));
  
  for (i = 0; i < 100; i++)
    assert(array2[i] == i);

  array3 = (int*)realloc(0, 100);

  assert(array3 != (int*)0);

  return 0;
}
