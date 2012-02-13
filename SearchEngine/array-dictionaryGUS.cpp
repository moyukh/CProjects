
// Implementation of a dictionary using an array and sequential search
// The array will be dynamically resized if necessary

#include <stdlib.h>

#include "array-dictionary.h"

// Constructor
ArrayDictionary::ArrayDictionary()
{
  // Add needed code
  index = 0;
  length = 1;
  array = new DictMember[length];
}

// Add a record to the dictionary. Returns false if key already exists
bool
ArrayDictionary::addRecord( KeyType key, DataType record)
{
  // Add needed code
  URLRecordList * temp= new URLRecordList();
  bool found = false;
  for (int i = 0; i<length; i++)
  {
	  if (strcmp(key, array[i].word) == 0)
	  {
		  temp = (URLRecordList *)record;
		  temp -> _next = array[i].list;
		  array[i].list = temp;
		  found = true;
		  return found;
	  }
		  
  }
  
  if (!found)
  {
	  array[index].word = key;
	  
	  array[index].list = (URLRecordList*)record;
  }
    
  
  
  return false;
}

// Find a key in the dictionary and return corresponding record or NULL
DataType
ArrayDictionary::findRecord( KeyType key)

{
  if (strcmp(key, array[0].word)==0)
  {
	  return array[0].list;
  }
	  
  return NULL;
}
