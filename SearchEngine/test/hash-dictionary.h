
// Implementation of a dictionary using a hash table

#include "dictionary.h"
//#include "webcrawl.h"

class HashDictionary : public Dictionary {
  // Add any member variables you need
 /* 
  struct DictMember{
	  KeyType word;
	  URLNumList *list;
  };
  */
  
  struct HashTableEntry {
  const char * _key;
  URLNumList *_data;
  HashTableEntry * _next;
};
  
  HashTableEntry **_buckets;
  
  int index;
  int length;
  
  
public:
  // Constructor
  enum { TableSize = 2039};
  
  HashDictionary();
  
  // Add a record to the dictionary. Returns false if key already exists
  bool addRecord( KeyType key, DataType record);
  
  // Find a key in the dictionary and return corresponding record or NULL
  DataType findRecord( KeyType key);  //=0;

  // Add other methods you may need
private:
	
	int hash( KeyType key);
	
  
};
