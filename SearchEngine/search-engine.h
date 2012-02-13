
#include "minihttpd.h"
//#include "dictionary.h"
#include"array-dictionary.h"
#include"hash-dictionary.h"
#include"avl-dictionary.h"
#include"bsearch-dictionary.h"
#include <sys/time.h>

// Types of dictionaries used
enum DictionaryType {
  ArrayDictionaryType,
  HashDictionaryType,
  AVLDictionaryType,
  BinarySearchDictionaryType
};

// Inherits from MiniHTTPD
class SearchEngine : public MiniHTTPD {
  Dictionary * _wordToURLList;
  
  // Add any other member variables you need
  URLRecord *urlarray;
  int numURL;
  int maxURL;
  char *dict;
  hrtime_t start, end, average;
  int searches;
 public:
  // Constructor for SearchEngine
  SearchEngine( int port, DictionaryType dictionaryType);

  // Called when a request arrives
  void dispatch( FILE * out, const char * requestLine );

};



