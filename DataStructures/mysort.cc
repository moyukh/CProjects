
#include "mysort.h"
#include <alloca.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

//
// Sort an array of element of any type
// it uses "compFunc" to sort the elements.
// The elements are sorted such as:
//
// if ascending != 0
//   compFunc( array[ i ], array[ i+1 ] ) <= 0
// else
//   compFunc( array[ i ], array[ i+1 ] ) >= 0
//
// See test_sort to see how to use mysort.
//
void mysort( int n,                      // Number of elements
	     int elementSize,            // Size of each element
	     void * array,               // Pointer to an array
	     int ascending,              // 0 -> descending; 1 -> ascending
	     CompareFunction compFunc )  // Comparison function.
{
  // Add your code here. Use any sorting algorithm you want.
  int i, j, x, y;
  void *temp = malloc(elementSize); //hold value for swap
  
  for (i = 0; i<n-1; i++)
  {
	  x = 0;
	  y = elementSize;
	  for (j = 0; j<n-1-i; j++)
	  {
		  if (ascending != 0)
		  {
			if (compFunc((char*)array+x, (char*)array+y) > 0) //descending
			{
				memcpy(temp, (char*)array+x, elementSize);//swap
				memcpy((char*)array+x, (char*)array+y, elementSize);
				memcpy((char*)array+y, temp, elementSize);
			}
		  }
		  else
		  {
			if (compFunc((char*)array+x, (char*)array+y) < 0) //ascending
			{
				memcpy(temp, (char*)array+x, elementSize);//swap
				memcpy((char*)array+x, (char*)array+y, elementSize);
				memcpy((char*)array+y, temp, elementSize);
			}
		  }
		  x+=elementSize;//increment by element size
		  y+=elementSize;
	  }
  }
  free(temp);
}

