
// CS251 Definitions for web-crawler

#include "HashTable.h"

// Stores a URL and the first 100 words
struct URLRecord {
  char * _url;          // URL of the document.
  char * _description;  // First 100 words of the document without tags
};

// Used to implement a list of URL indexes of the url array.
// There is one list of URLs for each word 
struct URLRecordList {
  int _urlRecordIndex;     // Index into the URL Array
  URLRecordList * _next;   // Pointer to the next URL Record in the list
};

class WebCrawler {
  // The url array stores all the URLs that have been scanned
  int _maxUrls;            // Max size of the URL array
  URLRecord * _urlArray;   // Array of URLs 
  int _headURL;            // Points to the next URL to scan by the web crawler
  int _tailURL;            // Next position unused in the urlArray
 
  HashTable<int> * _urlToUrlRecord;         //  maps a URL to its index in the URLArray 
  
  HashTable<URLRecordList *> *_wordToURLRecordList; // maps a word to a list of URLs
  
  //char * html; //This is the html document to be parsed

 public:
  // Create a web crawler object with the initial urls
  WebCrawler(int maxUrls, int nurlRoots, const char ** urlRoots);
  
  // crawl the web
  void crawl();

  // Write array of URLs and descriptions to file
  void writeURLFile(const char * urlFileName);

  // Write list of words with their urls to file
  void writeWordFile(const char *wordFileName);

  // Add any other objects you may need
  private:
  void description(char * html);
  void hyperlinks( char* html);
  void trim(char * s);
  char * mystrcpy( char * dest, const char * src );
};


