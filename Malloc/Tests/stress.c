#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#define space ((16*1024) - 16)

int
main( int argc, char **argv )
{
  if (argc < 2) {
    printf("Need 1 argument!!\n");
    return 1;
  }
  
  char* array[space / 24]; 
  int perm [space / 24];
  
  int i, j, k;
  int test = atoi(argv[1]);
  for (i = 1; i <= test; i++) {
    for (j = 0; j < space / (i*8 + 16); j++) {
      // malloc all 16 KB in i*8 B chunks
      array[j] = (char*)malloc(i*8);
      perm[j] = j;
    }
    
    
    for (k = 0; k < j; k++) {
      // produce random permutation
      int a = rand() % j;
      int b = rand() % j;
      if (a == b) continue;
      perm[a] = perm[a] ^ perm[b];
      perm[b] = perm[a] ^ perm[b];
      perm[a] = perm[a] ^ perm[b];
    }
    for (k = 0; k < j; k++) {
      assert(perm[k] < j);
      assert(array[perm[k]] != 0);
      free((char*)array[perm[k]]);
      array[perm[k]] = 0;
    }
    /*
      for (k = 0; k < space / (i*8 + 16); k++)
      free((char*)array[k]);
    */
  }
  
  pid_t pid = getpid();
  char command[1024];
  sprintf(command, "pmap %d\n", pid);
  system(command);

  return 0;
}


