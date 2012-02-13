
//
// CS251 Data Structures
// Hash Table template
//

#include <assert.h>
#include <stdlib.h>
#include <string.h>

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
{// TODO: Compute the hash number from the key string
	int hashNum = 0;
	//printf("hash %s", key);
	while (*key != '\0')
	{
		//printf("%c ", *key);
		hashNum += *key; //sum all chars of key
		*key++;
	}
	//printf("\n");
  return hashNum;
}

template <typename Data>
HashTable<Data>::HashTable()
{
  // TODO: Initialize the hash table
  //_buckets = new HashTableEntry<Data>;
  printf("Constructor\n");
  _buckets = (HashTableEntry<Data>**)malloc(sizeof(HashTableEntry<Data>*) * TableSize); //I really shoulda used new
  for (int i = 0; i<TableSize; i++)
  {
	 // printf("initialize %d ", i);
	  _buckets[i] = NULL; //initialize all values to null
  }
  //printf("\n");
}

template <typename Data>
bool HashTable<Data>::insertItem( const char * key, Data data)
{
  // TODO: Insert a key,data pair inside the hash table.
  // Return true if the entry already exists or false if
  // it does not exist.
  int hashNum = hash(key); //get hash number
  //printf("insert %s, hashnum:%d\n", key, hashNum);
  bool exists = false;
  HashTableEntry<Data> *temp = new HashTableEntry<Data>;
  HashTableEntry<Data> *check = new HashTableEntry<Data>;
  if (_buckets[hashNum] != NULL)
  {
	  //printf("make it?");
	  check = _buckets[hashNum];
	  while (check != NULL) //look if exists
	  {
		  if (strcmp(check->_key, key) == 0)
		  {
			  check->_data = data; //update data if it does
			  exists = true;
		  }
		  check = check->_next;
	  }
	  if (!(exists)) //if key does not exist, add to head of list
	  {
		  temp->_data = data;
		  temp->_key = key;
		  temp->_next = _buckets[hashNum];
		  _buckets[hashNum] = temp;
		  exists = false;
	  }
  }
  else
  {
	  //if nothing exists, create a new entry
	  _buckets[hashNum] = new HashTableEntry<Data>;
	  _buckets[hashNum]->_data = data;
	  _buckets[hashNum]->_key = key;
	  _buckets[hashNum]->_next = NULL;
	  exists = false;
  }
  
  return exists;
}

template <typename Data>
bool HashTable<Data>::find( const char * key, Data * data)
{
  // TODO: Find the data associated to a key. The data is
  // stored in the *data passed as parameter.
  // Return true if the entry is found or false otherwise.
  int hashNum = hash(key);
  if (_buckets[hashNum] != NULL)
  {
	  //Data* temp = &_buckets[hashNum]->_data;
	  
	  *data = _buckets[hashNum]->_data; //update data if found
	  //printf("key:%s, data:%d", key, *data);
	  return true;
  }
  return false;
}

template <typename Data>
bool HashTable<Data>::removeElement(const char * key)
{
  // TODO: Remove the element that has this key from the hash table.
  // Return true if the entry is found or false otherwise.
  int hashNum = hash(key);
  HashTableEntry<Data> *check = new HashTableEntry<Data>;
  if (_buckets[hashNum] != NULL)
  {
	  
	  if (strcmp(_buckets[hashNum]->_key, key)==0) //first element is key
	  {
		  _buckets[hashNum] = _buckets[hashNum] -> _next; //remove
		  return true;
	  }
		  
	  check = _buckets[hashNum];
	  while (check -> _next != NULL) //look if exists
	  {
		  if (strcmp(check->_next->_key, key) == 0)
		  {
			  check->_next = check->_next->_next; //remove element containing key
			  return true;
		  }
		  check = check->_next;
	  }
	  //_buckets[hashNum] = NULL;
  }
  else
	  return false;
}

/////////////////////////////////////
//
// Class used to iterate over the hash table
//
/////////////////////////////////////

template <typename Data>
class HashTableIterator {
  int _currentBucket; // Current bucket that is being iterated
  HashTableEntry<Data> *_currentEntry; // Current entry iterated
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
  _currentEntry = _hashTable -> _buckets[0];
}

template <typename Data>
bool HashTableIterator<Data>::next(const char * & key, Data & data)
{
  // TODO: Returns the next element in the hash table.
  // The key and data values are stored in the references passed
  // as argument. It returns true if there are more entries or
  // false otherwise.
  //bool moreEntries = true;
  printf("%d", _currentBucket);
  if (_currentEntry == NULL && _currentBucket < _hashTable->TableSize)
  {
	 // printf("test");
	  while(_currentEntry == NULL && _currentBucket < _hashTable->TableSize) //find element
	  {
		  //printf("%d\n", _currentBucket);
		  _currentBucket++;
		  _currentEntry = _hashTable->_buckets[_currentBucket];
	  }
	  	  
	  if (_currentBucket == _hashTable->TableSize) //check if whole table has been checked
		  return false;
	  
  }
  //update parameters
  key = _currentEntry -> _key;
  data = _currentEntry -> _data;
  _currentEntry = _currentEntry -> _next;
  
  return true;
}
