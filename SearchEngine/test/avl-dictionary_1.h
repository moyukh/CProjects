
// Implementation of a dictionary using an AVL tree
// AVL Trees are balanced binary trees 

#include "dictionary.h"
//#include "webcrawl.h"  //note that this is also included in array

class AVLDictionary : public Dictionary {
  // Add any member variables you need
  struct DictMember{
    KeyType word;
    URLNumList * list;
    int height;
    DictMember *left;
    DictMember *right;
    DictMember *parent;
  };
  
  DictMember *root;

public:
  // Constructor
  AVLDictionary();
  
  // Add a record to the dictionary. Returns false if key already exists
  bool addRecord( KeyType key, DataType record);
  
  // Find a key in the dictionary and return corresponding record or NULL
  DataType findRecord( KeyType key);  //= 0;


  void restructure(DictMember *n);
  // Add other methods you may need
};
