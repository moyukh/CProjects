#include <string.h>
#include <fstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "search-engine.h"
//#include "webcrawl.h"

SearchEngine::SearchEngine( int port, DictionaryType dictionaryType):
  MiniHTTPD(port)
{
  searches = 0;
  // Create dictionary of the indicated type
  if(dictionaryType == 0){
    _wordToURLList = new ArrayDictionary();
    dict = "Array Dictionary";
  }
  if(dictionaryType == 1){
    _wordToURLList = new HashDictionary();
    dict = "Hash Dictionary";
  }
  if(dictionaryType == 2){
    _wordToURLList = new AVLDictionary();
    dict = "AVL Dictionary";
  }
  if(dictionaryType == 3){
    _wordToURLList = new BinarySearchDictionary();
    dict = "Binary Search Dictionary";
  } 
  //Read in word.txt into dictionary
  FILE *inFile;
  
  inFile = fopen("word.txt", "r");
  char *buffer = new char[11000];
  char *in = new char[50];
  char *word = new char[50];
  URLNumList *add;
  
  add = new URLNumList();
  add -> _num = 15;
  add -> _next = NULL;

  //_wordToURLList -> addRecord("Moyukh", add);

  if(inFile != NULL)
  {
	  while(fgets(buffer, 11000, inFile))
	  {
		   in = strtok(buffer, " ");
		   word = strdup(in);
		   //printf("word:%s\n", word);
		   while (in != NULL)
		   {
			   in = strtok(NULL, " \n");
			   
			   if (in != NULL)
			   {
			     //printf("*%s*\n", in);
				   add = new URLNumList();
				   add -> _num = atoi(in);
				   
				   add -> _next = NULL;			   
				   _wordToURLList -> addRecord(word, add);			   
				   //printf("%s %d\n", word, add -> _num);
			   }
		   }
	  }
  }
  
  fclose(inFile);
  
  urlarray = new URLRecord[5000];
  inFile = fopen("url.txt", "r");
  //char *num = new char[5];
  char *url = new char[400];
  
  
  int j=0;
  
  if (inFile != NULL)
  {
	  
	  while(fgets(buffer, 11000, inFile))
	  {
		  in = strtok(buffer, " ");
		 // num = strdup(in);
		  //printf("%s\n", in);
		  j = atoi(in);
		  
		  
		  in = strtok(NULL, "\n");
		  urlarray[j]._url = strdup(in);
		  //printf("%s\n", urlarray[j]._url);
		  
		  fgets(buffer, 11000, inFile);
		  
		  urlarray[j]._description = strdup(buffer);
		  //printf("%s\n", urlarray[j]._description);
		  
		  fgets(buffer, 11000, inFile);
	  }
  }
  fclose(inFile);
  maxURL = j;
  
  
  
  
  // Populate dictionary and sort it if necessary
}

void
SearchEngine::dispatch( FILE * fout, const char * documentRequested)
{
  if (strcmp(documentRequested, "/")==0) {
    // Send initial form
    fprintf(fout, "<TITLE>CS251 Search</TITLE>\r\n");
    fprintf(fout, "<CENTER><H1><em>Boiler Search</em></H1>\n");
    fprintf(fout, "<H2>\n");
    fprintf(fout, "<FORM ACTION=\"search\">\n");
    fprintf(fout, "Search:\n");
    fprintf(fout, "<INPUT TYPE=\"text\" NAME=\"word\" MAXLENGTH=\"80\"><P>\n");
    fprintf(fout, "</H2>\n");
    fprintf(fout, "</FORM></CENTER>\n");
    return;
  }
  start = gethrtime();
  // TODO: The words to search in "documentRequested" are in the form
  // /search?word=a+b+c
  //
  // You need to separate the words before search
  // Search the words in the dictionary and find the URLs that
  // are common for al the words. Then print the URLs and descriptions
  // in HTML. Make the output look nicer.

  // Here the URLs printed are hardwired
  int i = 0;
  int nurls;
  char * words = strdup(documentRequested);
  int numofwords = 0;
  words = words+13;
  char * begin = strdup(words);
  int x = 0;
  while(words[x]!='\0'){
    words[x] = tolower(words[x]);
    x++;
  }
  char* tok = strtok(words, "+");
  int *urlcounter = new int[maxURL];
  for(x = 0;x<=maxURL;x++){
    urlcounter[x] = 0;
  }
  x=0;
  while(begin[x] != '\0'){
    if(begin[x] == '+')
      begin[x] = ' ';
    x++;
  }
  x=0;
  char **urls = (char**)malloc(sizeof(char*)*maxURL);
  
  char *description[maxURL];

  while(tok!=NULL){
    //while(tok[x]!='\0'){
    //tok[x] = tolower(tok[x]);
    //x++;
    //}
    URLNumList* current = (URLNumList *)_wordToURLList -> findRecord(tok);
    while(current!=NULL){
      urlcounter[current->_num]++;
      current = current->_next;
    }
    tok = strtok(NULL,"+");
    numofwords++;
  }
  //printf("urlcounter[53] = %d\n",urlcounter[53]);
  nurls = 0;
  for(int j = 0; j<=maxURL; j++){
    if(urlcounter[j] == numofwords){
      urls[i] = urlarray[j]._url;
      description[i] = urlarray[j]._description;
      i++;
    }
  }
  nurls = i;
  end = gethrtime();
  searches++;
  average = (average*(searches-1) + (end-start))/searches;
  //printf("number of words  = %d\ni = %d\nmaxURL = %d\nnurls = %d\ndict= %s",numofwords,i,maxURL,nurls,dict);
  //printf("dict = %s",dict);
  // = {
    //"Computer Science Department. Purdue University.",
    //"CS251 Data Structures"
  //};
  

  //run findrecord for numofwords times updating urlcounter each time a url 
  //is listed for the searched word, then compare urlcounter[j] to numofwords
  //if equal, update urls[i] and description[i] with urlarray[j]._url and
  //urlarray[j]._description
  

  /*
  URLNumList* current = (URLNumList *)_wordToURLList -> findRecord(begin); //print values
  int i = 0;
  
  while (current != NULL)
  {
	  urls[i] =  urlarray[current -> _num]._url;
	  description[i] = urlarray[current -> _num]._description;
	  
	  //printf("%s\n", description[i]);
	  
	  current = current -> _next;
	  i++;
  }
  
  nurls = i;
  */
  /*
  for(i = 0; i<nurls; i++)
  {
	  printf("%s\n", description[i]);
  }*/

  fprintf( stderr, "Search for words: \"%s\"\n", begin);

  fprintf( fout, "<TITLE>Search Results</TITLE>\r\n");
  fprintf( fout, "<H1> <Center><em>Boiler Search</em></H1>\nwith %s took %lld nsec Average Time = %lld nsec",dict,(end-start),average);
  fprintf( fout, "<H2> Search Results for \"%s\"</center></H2>\n", begin );

  for ( int i = 0; i < nurls; i++ ) {
    fprintf( fout, "<h3>%d. <a href=\"%s\">%s</a><h3>\n", i+1, urls[i], urls[i] );
    fprintf( fout, "<blockquote>%s<p></blockquote>\n", description[i] );
  }
  
  /*
  while (current != NULL)
  {
	 // urls[i] =  strdup(urlarray[current -> _num]._url);
	 // description[i] = strdup(urlarray[current -> _num]._description);
	  
	  
	  fprintf( fout, "<h3>%d. <a href=\"%s\">%s</a><h3>\n", i+1, urlarray[current -> _num]._url, urlarray[current -> _num]._url );
	  fprintf( fout, "<blockquote>%s<p></blockquote>\n", urlarray[current -> _num]._description);
	  //printf("%s\n", description[i]);
	  
	  current = current -> _next;
	  i++;
  }*/
  

  // Add search form at the end
  fprintf(fout, "<HR><H2>\n");
  fprintf(fout, "<FORM ACTION=\"search\">\n");
  fprintf(fout, "Search:\n");
  fprintf(fout, "<INPUT TYPE=\"text\" NAME=\"word\" MAXLENGTH=\"80\"><P>\n");
  fprintf(fout, "</H2>\n");
  fprintf(fout, "</FORM>\n");
}

void
printUsage()
{
  const char * usage =
    "Usage: search-engine port (array | hash | avl | bsearch)\n"
    "  It starts a search engine at this port using the\n"
    "  data structure indicated. Port has to be larger than 1024.\n";

  fprintf(stderr, usage);
}

int main(int argc, char ** argv)
{
  if (argc < 3) {
    printUsage();
    return 1;
  }

  // Get port
  int port;
  sscanf( argv[1], "%d", &port);

  // Get DictionaryType
  const char * dictType = argv[2];
  DictionaryType dictionaryType;
  if (!strcmp(dictType, "array")) {
    dictionaryType = ArrayDictionaryType;
  }
  else if (!strcmp(dictType, "hash")) {
    dictionaryType = HashDictionaryType;
  }
  else if (!strcmp(dictType, "avl")) {
    dictionaryType = AVLDictionaryType;
  }
  else if (!strcmp(dictType, "bsearch")) {
    dictionaryType = BinarySearchDictionaryType;
  }
  else {
    printUsage();
    return 0;
  }
  
  SearchEngine httpd(port, dictionaryType);
  
  httpd.run();

  return 0;
}
