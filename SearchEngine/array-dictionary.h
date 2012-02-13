
// Implementation of a dictionary using an array and sequential search
// The array will be dynamically resized if necessary

#include "dictionary.h"
//#include "webcrawl.h"

class ArrayDictionary : public Dictionary {
  // Add any member variables you need
  struct DictMember{
	  KeyType word;
	  URLNumList *list;
  };
  
  DictMember *array;
  DictMember *temparray;
  int index;
  int length;
  
public:
  // Constructor
  ArrayDictionary();
  
  // Add a record to the dictionary. Returns false if key already exists
  bool addRecord( KeyType key, DataType record);
  
  // Find a key in the dictionary and return corresponding record or NULL
  // Use sequential search
  DataType findRecord( KeyType key);

  // Add other methods you may need
};
