
// Implementation of a dictionary using an array and sequential search
// The array will be dynamically resized if necessary

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "array-dictionary.h"

// Constructor
ArrayDictionary::ArrayDictionary()
{
  // Add needed code
  index = 0;
  length = 5000;
  array = new DictMember[length];
}

// Add a record to the dictionary. Returns false if key already exists
bool
ArrayDictionary::addRecord( KeyType key, DataType record)
{
  // Add needed code
  URLNumList * temp= new URLNumList();
  bool found = false;
  for (int i = 0; i<index; i++)
  {
	  if (strcmp(key, array[i].word) == 0)
	  {
		  printf("%d current\n", array[i].list -> _num);
		  temp = (URLNumList*)record;
		  
		  
		  
		  if (temp -> _num <= array[i].list -> _num) //insert before lesser value
		  {
			  temp -> _next = array[i].list;
			  array[i].list = temp;
		  }
		  else
		  {
			  URLNumList *iterator = array[i].list;
			  while(iterator->_next != NULL && iterator->_next->_num < temp -> _num) //insert after higher value
			  {
				  iterator = iterator->_next;
			  }
			  temp->_next = iterator->_next;
			  iterator->_next = temp;
			  
		  }
		  
		  
		  found = true;
		  /*
		  int j = array[i].list -> _num;
		  printf("%d inserted\n", j);
		  j = array[i].list -> _next -> _num;
		  printf("%d there\n", j);*/
		  return found;
	  }
		  
  }
  
  if (!found)
  {
	  array[index].word = strdup(key);
	  
	  array[index].list = (URLNumList*)record;
	  
	  //int j = array[index].list -> _num;
	 // printf("%d inserted\n", j);
	  index++;
  }
    
  
  
  return false;
}

// Find a key in the dictionary and return corresponding record or NULL
DataType
ArrayDictionary::findRecord( KeyType key)

{
	for (int i = 0; i<index; i++)
	{
		printf("%d\n", i);
		  if (strcmp(key, array[i].word)==0)
		  {
			  printf("found at %d\n", i);
			  return array[i].list;
		  }
	}
	  
  return NULL;
}
