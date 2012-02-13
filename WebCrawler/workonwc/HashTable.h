
//
// CS251 Data Structures
// Hash Table template
//

#include <assert.h>
#include <stdlib.h>
#include <string.h>
//#include "webcrawler.h"

///////////////////////////////////////////
//
// Class that defines a template for a hash table
// The keys are of type const char * and the data
// can be any type.
//
// Use like this:
//  HashTable hashTableInt<int>;
//  
//
///////////////////////////////////////////

// Each hash entry stores a key, object pair
template <typename Data>
struct HashTableEntry {
  const char * _key;
  Data _data;
  HashTableEntry * _next;
};

// This is a Hash table that maps string keys to objects of type Data
template <typename Data>
class HashTable {
 public:
  // Number of buckets
  enum { TableSize = 2039};
  
  // Array of the hash buckets.
  HashTableEntry<Data> **_buckets;
  
  // Obtain the hash code of a key
  int hash(const char * key);
  
 public:
  HashTable();
  
  // Add a record to the hash table. Returns true if key already exists.
  // Substitute content if key already exists.
  bool insertItem( const char * key, Data data);

  // Find a key in the dictionary and place in "data" the corresponding record
  // Returns false if key is does not exist
  bool find( const char * key, Data * data);

  // Removes an element in the hash table. Return false if key does not exist.
  bool removeElement(const char * key);
};

template <typename Data>
int HashTable<Data>::hash(const char * key)
{
  int x = 0;
  for(int i = 0; i<strlen(key); i++)
    x = x + 2053*key[i];
  x = x%TableSize;
  return x;
}

template <typename Data>
HashTable<Data>::HashTable()
{
  _buckets = new (HashTableEntry<Data>*)[TableSize];
  for(int i = 0; i<TableSize; i++)
  {
	  _buckets[i] = NULL;
  }
  // TODO: Initialize the hash table
}


template <typename Data>
bool HashTable<Data>::insertItem( const char * key, Data data)
{
  int index = hash(key);
  HashTableEntry<Data> * current;
  HashTableEntry<Data> * entry = new HashTableEntry<Data>;
  entry->_key = key;
  entry->_data = data;
  entry->_next = NULL;
  if( _buckets[index] == NULL)
    _buckets[index] = entry;
  else{//bucket is not empty
    current = _buckets[index];
    /*
    while(strcmp(current->_key,key)!=0){ //May want to use mystrcmp
      if(current->_next == NULL){
	current->_next = entry;
	return false;
      }
      current = current->_next;
    }
    current->_data = data;
    return true;
    */
    
    while (current != NULL)
    {
	    if(strcmp(current -> _key, key) == 0)
	    {
		    current -> _data = data;
		    return true;
	    }
	    else if (current -> _next == NULL)
	    {
		    current -> _next = entry;
		    return false;
	    }
	    current = current -> _next;
    }
    
    
  }
  // TODO: Insert a key,data pair inside the hash table.
  // Return true if the entry already exists or false if
  // it does not exist.

  return false;
}

template <typename Data>
bool HashTable<Data>::find( const char * key, Data * data)
{
  // TODO: Find the data associated to a key. The data is
  // stored in the *data passed as parameter.
  // Return true if the entry is found or false otherwise.
  int index = hash(key);
  printf("hashnum %s = %d\n", key, index);
  HashTableEntry<Data> * current;
  if(_buckets[index] == NULL)
    return false;
  else{
    current = _buckets[index];
    /*
    if (_buckets[1276]!= NULL)
    {
	    printf("BUCKETT:%s\n", _buckets[1276]->_key);
    }*/
    
    /*
    while(strcmp(comp,key) != 0){
      current = current->_next;
      if(current == NULL) //loop on current -> null, strcmp for good shizz
	return false;
    }
    *data = current->_data;	  
    return true;*/
    
    while(current != NULL)
    {
	    //char *comp = strdup(current->_key);
	    printf("searched key:%s\n", key);
	    printf("existing key:%s\n\n", current-> _key);
	    
	    if(strcmp(current -> _key, key)==0)
	    {
		    *data = current->_data;	  
		    return true;
	    }
	    current = current -> _next;
    }
    
  }
  return false;
}

template <typename Data>
bool HashTable<Data>::removeElement(const char * key)
{
  // TODO: Remove the element that has this key from the hash table.
  // Return true if the entry is found or false otherwise.
  int index = hash(key);
  HashTableEntry<Data> * current = _buckets[index];
  HashTableEntry<Data> * tmp;
  if(current == NULL)
    return false;
  else if(strcmp(current->_key,key)==0){
    _buckets[index] = current->_next;
    free(current);
    return true;
  }
  while(current->_next != NULL && strcmp(current->_next->_key,key)!=0){
    current = current->_next;
  }
  if(current->_next == NULL)
      return false;
  else{
    tmp = current->_next;
    current = current->_next->_next;
    free(tmp);
    return true;
  }
  
  return false;
}

/////////////////////////////////////
//
// Class used to iterate over the hash table
//
/////////////////////////////////////

template <typename Data>
class HashTableIterator {
  enum { TableSize = 2039};
  int _currentBucket; // Current bucket that is being iterated
  HashTableEntry<Data> * _currentEntry; // Current entry iterated
  bool tableEnd; //turns true when hashTable end is reached
  HashTable<Data> * _hashTable;  // Pointer to the hash table being iterated
 public:
  HashTableIterator(HashTable<Data> * hashTable);
  bool next(const char * & key, Data & data);
};

template <typename Data>
HashTableIterator<Data>::HashTableIterator(HashTable<Data> * hashTable)
{
  // TODO: Initialize iterator. "hashTable" is the table to be iterated.
  _hashTable = hashTable;
  _currentBucket = 0;
  _currentEntry = _hashTable->_buckets[_currentBucket];
  tableEnd = false;
}

template <typename Data>
bool HashTableIterator<Data>::next(const char * & key, Data & data)
{
  // TODO: Returns the next element in the hash table.
  // The key and data values are stored in the references passed
  // as argument. It returns true if there are more entries or
  // false otherwise.
  // This is wrong.  Apparently to pass cases, it must return true if there
  // is an entry and false if not
  
  if(tableEnd == true)
    return false;
  while(_currentEntry == NULL){
    _currentBucket++;
    if(_currentBucket >= TableSize){
      tableEnd = true;
      return false;
    }
    _currentEntry = _hashTable->_buckets[_currentBucket];
  }//_currentEntry is not NULL, stores key and data now
  data = _currentEntry->_data;
  key = _currentEntry->_key;
  //ignore: go to next entry and return true, otherwise return false
  //check for HashTableEntry->_next in currentBucket
  //correct: get next entry if possible and return true
  if(_currentEntry->_next != NULL){
    _currentEntry = _currentEntry->_next;
    return true;
  }
  else{
    _currentBucket++;//next bucket
    _currentEntry = _hashTable->_buckets[_currentBucket];
      }
  while(_currentEntry == NULL){ //iterates table to find next entry
    _currentBucket++;
    if(_currentBucket >=  TableSize){ //end of table
      tableEnd = true;
      return true;
    }
    _currentEntry = _hashTable->_buckets[_currentBucket];
  }
  return true;
}
