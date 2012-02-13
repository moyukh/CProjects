#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

int
main( int argc, char **argv )
{
  int i;
  char* array = (char*)calloc(100, sizeof(char));
  for (i = 0; i < 100; i++)
    assert(array[i] == 0);

  return 0;
}
