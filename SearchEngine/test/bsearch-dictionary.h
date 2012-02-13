
// Implementation of a dictionary using an array and binary search
// The class inherits from ArrayDictionary

#include "dictionary.h"

class BinarySearchDictionary : public Dictionary {
  // Add any member variables you need
  struct DictMember{
	  KeyType word;
	  URLNumList *list;
  };
  DictMember *temparray;
  DictMember *array;
  int index;
  int length;
  bool sorted;

public:
  // Constructor
  BinarySearchDictionary();
  
  // No need for addRecord since it will use the one in ArrayDictionary
  
  bool addRecord( KeyType key, DataType record);

  // Find a key in the dictionary and return corresponding record or NULL
  // Use binary search
  DataType findRecord( KeyType key);

  // Sort array using heap sort.
  void sort();

  // Add other methods you may need
};

