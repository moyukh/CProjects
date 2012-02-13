
#include <stdio.h>
#include <assert.h>
#include "ListGeneric.h"

int
main(int argc, char **argv)
{
  //////////////////////////////////////
  // testing lists for ints
  
  ListGeneric<int> * listInt = new ListGeneric<int>();

  listInt->insert(8);
  listInt->insert(9);

  int val;
  bool e;
  e = listInt->remove(val);
  assert(e);
  assert(val==9);

  e = listInt->remove(val);
  assert(e);
  assert(val==8);

  //////////////////////////
  // testing lists for strings
  
  ListGeneric<const char *> * listString = new ListGeneric<const char *>();

  listString->insert("hello");
  listString->insert("world");

  const char * s;
  e = listString->remove(s);
  assert(e);
  assert(!strcmp(s,"world"));

  e = listString->remove(s);
  assert(e);
  assert(!strcmp(s,"hello"));

  ///////////////////////////
  // testing iterator for lists
  const char * (array[]) = {"one","two","three","four","five","six"};
  int n = sizeof(array)/sizeof(const char*);

  int i;
  for (i=0;i<n;i++) {
    listString->insert(array[i]);
  }
  
  ListGenericIterator<const char *> iterator(listString);
  while (iterator.next(s)) {
    printf(">>%s\n",s);
    i--;
    assert(!strcmp(s,array[i]));
  }

  printf("Tests passed!\n");

}
