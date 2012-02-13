
// Implementation of a dictionary using a hash table

#include <stdlib.h>
#include "hash-dictionary.h"
#include<stdio.h>

// Constructor
HashDictionary::HashDictionary()
{
  // Add needed code
   _buckets = new (HashTableEntry*)[TableSize];
  for(int i = 0; i<TableSize; i++)
  {
	  _buckets[i] = NULL;
  }
}

// Add a record to the dictionary. Returns false if key already exists
bool 
HashDictionary::addRecord( KeyType key, DataType record)
{
	int index = hash(key);
	  HashTableEntry * current;
	  HashTableEntry * entry = new HashTableEntry();
	  URLNumList * temp= new URLNumList();
	  entry->_key = key;
	  entry->_data = (URLNumList *)record;
	  entry->_next = NULL;
	  if( _buckets[index] == NULL)
	    _buckets[index] = entry;
	  else{//bucket is not empty
	    current = _buckets[index];
	    
	    while (current != NULL)
	    {
		    if(strcmp(current -> _key, key) == 0)
		    {
			    temp = (URLNumList*)record;
			     if (temp -> _num <= current -> _data -> _num) //insert before lesser value
				  {
					  temp -> _next = current -> _data;
					  current -> _data = temp;
					  //printf("insert: %d\n", temp -> _num);
				  }
				  else
				  {
					  URLNumList *iterator = current -> _data;
					  while(iterator->_next != NULL && iterator->_next->_num < temp -> _num) //insert after higher value
					  {
						  iterator = iterator->_next;
					  }
					  temp->_next = iterator->_next;
					  iterator->_next = temp;
					//  printf("insert: %d\n", temp -> _num);
				  }
			    
			    
			    //current -> _data = (URLNumList *)record;
			    return true;
		    }
		    else if (current -> _next == NULL)
		    {
			    current -> _next = entry;
			    return false;
			    printf("insert: %d\n", temp -> _num);
		    }
		    current = current -> _next;
	    }
	    
	    
	  }
	  return false;
}

// Find a key in the dictionary and return corresponding record or NULL
DataType
HashDictionary::findRecord( KeyType key)
{
	// TODO: Find the data associated to a key. The data is
  // stored in the *data passed as parameter.
  // Return true if the entry is found or false otherwise.
  int index = hash(key);
 // printf("hashnum %s = %d\n", key, index);
  HashTableEntry * current;
  if(_buckets[index] == NULL)
    return NULL;
  else{
    current = _buckets[index];
    
    while(current != NULL)
    {
	    //char *comp = strdup(current->_key);
	   // printf("searched key:%s\n", key);
	    //printf("existing key:%s\n\n", current-> _key);
	    
	    if(strcmp(current -> _key, key)==0)
	    {
		    //*data = current->_data;	  
		    return current->_data;
	    }
	    current = current -> _next;
    }
    
  }

  // Add needed code
  return NULL;
}

int
HashDictionary::hash( KeyType key)
{
	int x = 0;
	  for(int i = 0; i<strlen(key); i++)
	    x = x + 2053*key[i];
	  x = x%TableSize;
	  return x;
}


