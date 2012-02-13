
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include<fstream.h>
#include "webcrawler.h"
#include "openhttp.h"
// Add your implementation here

WebCrawler::WebCrawler(int maxUrls, int nurlRoots, const char ** urlRoots)
{
  _maxUrls = maxUrls;
  _urlArray = new URLRecord[maxUrls+1];
  int i = 0;
  _urlToUrlRecord = new HashTable<int>;
  _wordToURLRecordList = new HashTable<struct URLRecordList *>;
  
  for (i = 0; i<nurlRoots; i++)
    {
      //printf("%s\n", urlRoots[i]);
      //printf("%d\n", strlen(urlRoots[i]));
      (_urlArray+i)->_url = (char *)malloc(strlen(urlRoots[i]) +1);
      strcpy((_urlArray+i)->_url,urlRoots[i]);
      
      _urlToUrlRecord -> insertItem(strdup(urlRoots[i]), i);
      
      //(_urlArray+i)->_url = urlRoots[i];
     // printf("%s\n", (_urlArray+i)->_url);
      
    }
  _headURL = 0;
  _tailURL = i;
  
}

void WebCrawler::crawl()
{
  const char * url;
  char * tmp = new char;
  
  while (_headURL < _tailURL && _headURL < _maxUrls)
    {
      //url = _urlArray[_headURL]._url;
      //printf("urlone: %s\n", _urlArray[_headURL]._url);
      char * website = (char*)malloc(5000000);
      
      char contentType[ MaxLineLength ];
      
      
      
      FILE *f;
      f = openhttp( _urlArray[_headURL]._url, contentType );
      if ( f == NULL ) {
	_headURL++;
	continue ;
      }
      
      contentType[9]=0;
      printf("ctype: %s\n", contentType);
      if(strcmp(contentType,"text/html") != 0){
	printf("Error: document is not text/html\n");
	_headURL++;
	continue;
      }
      
      char line[ MaxLineLength ];
      while ( fgets( line,MaxLineLength, f ) ) {
	strcat(website, line);
      }
      
      fclose(f);   
      
      
      //free(f);
      
      // printf("description url before call %s: \n", _urlArray[_headURL]._url);
      //printf("%s\n", website);
      //_urlArray[_headURL]._description = strdup(website);
      
      
      
      
      
      
      //printf("check link %s\n", result);
      if (_tailURL < _maxUrls){
	      
	      char * links = (char*)malloc(5000000);
	     bool found = false;
	      if (links == NULL)
		      printf("\nNULLLLL\n");
	      
	      strcpy(links, website);
	      
	      hyperlinks(links);
	    //  printf("%s\n", links);
	      
	      char delims[] = "\n";
	      char *result = strtok( links, delims );
	      
		while( result != NULL && _tailURL < _maxUrls) {
		 // printf("tail = %d check link %s\n", _tailURL, result);
		  found = false;
		  //look if link is unique
		  for (int w = 0; w<_tailURL; w++)
		  {
			  if (strcmp(_urlArray[w]._url, result) == 0)
			  {
				  found = true;
				  break;
			  }
		  }
		  
		  if (!found)
		  {
		  
			  _urlArray[_tailURL]._url = strdup(result);
			  _urlToUrlRecord -> insertItem(result, _tailURL);
		//	  printf("%s\n", _urlArray[_tailURL]._url);
			  _tailURL++;
		  }
		/*  
		  if (_urlToUrlRecord -> find(result, &_tailURL))
		    {
		      printf("found\n");
		    }*/
		  
		  result = strtok( NULL, delims );
	}
	free(links);
      }
      
      description(website);
      _urlArray[_headURL]._description = strdup(website);
      
      int i = 0;
      int spaceCount = 0;
        while(_urlArray[_headURL]._description[i] != '\0' && spaceCount < 100)
	    {
	      if (_urlArray[_headURL]._description[i] == ' ')
		spaceCount++;
	      i++;
	    }
	  
	  if (spaceCount == 100)
	    _urlArray[_headURL]._description[--i] = '\0';
	  
	  char delim[] = " ";
	  char *word = strtok(website, delim);
	  
	  struct URLRecordList **urlistptr;
	  struct URLRecordList *newUrlRecord;
	  struct URLRecordList *temp;
	  
	  while ( word != NULL)
	  {
		  newUrlRecord = new struct URLRecordList;
		  
		  newUrlRecord -> _urlRecordIndex = _headURL;
		  newUrlRecord -> _next = NULL;
		  
		  int hashNum = _wordToURLRecordList -> hash(word);
  
		  if (_wordToURLRecordList -> _buckets[hashNum] != NULL)
		  {
			  //Data* temp = &_buckets[hashNum]->_data;
			 // printf("asd find %d\n", _wordToURLRecordList -> _buckets[hashNum]->_data -> _urlRecordIndex);
			  
			  //data = _buckets[hashNum]->_data;
			  
			  _wordToURLRecordList -> _buckets[hashNum]->_data -> _next = newUrlRecord;
			  //_wordToURLRecordList -> _buckets[hashNum]->_next -> _urlRecordIndex = _headURL;
			  
			//  printf("dblkey:%s, data:%d\n", _wordToURLRecordList -> _buckets[hashNum]->_key, _wordToURLRecordList -> _buckets[hashNum]->_data -> _urlRecordIndex);
		  }
		  
		  /*
		  if (_wordToURLRecordList -> find(word, temp))
		  {
			  printf("found\n");
			 // temp = *urlistptr;
			  printf("num %u\n", temp);
			  temp -> _next = newUrlRecord;
		  }*/
		  else
		  {
			  printf("insert:%s \n", word);
			  _wordToURLRecordList -> insertItem(strdup(word), newUrlRecord);
		  }
		  
		  word = strtok(NULL, delim);
		  free (newUrlRecord);
	  }
      
      
      
      /*mystrcpy(tmp, html);
	_urlArray[_headURL]._description = (char *)malloc(strlen(tmp) + 1);
	_urlArray[_headURL]._description = tmp;
	printf("%s\n", _urlArray[_headURL]._description);
	
	//hyperlinks(url);
	
	char urls[] = "http://www.purdue.edu/ssinfo\n/hiring/head\n/hiring/lecturer.sxhtml\nhttp://www.cs.purdue.edu/news/12-15-06Libestpaper.htm\nhttp://www.cs.purdue.edu/news/12-14-06IBMocr.htm\nhttp://www.cs.purdue.edu/news/12-8-06postercontest.html\n/announce\nhttps://portals.cs.purdue.edu/csrt/\nhttps://nemo.cs.purdue.edu/wjg/UTAapp.htm\n";
	
	
	
	char delims[] = "\n";
	const char *result = NULL;
	result = strtok( urls, delims );
	while( result != NULL  && _tailURL < _maxUrls) {
	if (strncmp(result, "http", 4) != 0)
	{
	_urlArray[_tailURL]._url = (char *)malloc(strlen(result) + 1);
	//strcpy(_urlArray[_tailURL]._url,result);
	_urlArray[_tailURL]._url = strdup(result);
	_urlToUrlRecord -> insertItem(result, _tailURL);
	//strcat(_urlArray[_tailURL]._url, result);
	}
	else
	{
	       _urlArray[_tailURL]._url = (char *)malloc(strlen(result) + 1);
	       //strcpy(_urlArray[_tailURL]._url,result);
	       _urlArray[_tailURL]._url = strdup(result);
	       _urlToUrlRecord -> insertItem(result, _tailURL);
       }
       if (_urlToUrlRecord -> find(result, &_tailURL))
       {
	       printf("found\n");
       }
       printf("%s\n", _urlArray[_tailURL]._url);
       _tailURL++;
       result = strtok( NULL, delims );
   }
   
   result = NULL;
   //char words[] = tmp;
  // strcpy (words, tmp);
   strcpy(delims, " ");
   result = strtok(tmp, delims);
   while (result != NULL)
   {
	   printf("%s\n", result);
	   struct URLRecordList * record = new URLRecordList;
	   record -> _urlRecordIndex = _headURL;
	   record -> _next = NULL;
	   _wordToURLRecordList -> insertItem(result, record); 
	   result = strtok(NULL, delims);
   }
		*/
		
      _headURL++;
      
      free(website);
      free(contentType);
      //free(links);
      //free(delims);
      //free(result);
      printf("%d  %d\n", _headURL, _tailURL);
    }
    writeURLFile("url.txt");
    writeWordFile("word.txt");
}

void WebCrawler::writeURLFile(const char * urlFileName)
{
	ofstream outFile;
	outFile.open (urlFileName);
	
	for (int i = 0; i<_maxUrls; i++)
	{
		outFile<<i+1<<" "<<_urlArray[i]._url<<"\n";
		outFile<<_urlArray[i]._description<<"\n\n";
	}
	
	outFile.close();
	return;
}

void WebCrawler::writeWordFile(const char *wordFileName)
{
	ofstream outFile;
	
	
	const char * word;
	struct URLRecordList * rec;
	
	HashTableIterator<URLRecordList *> iterator = HashTableIterator<URLRecordList *>(_wordToURLRecordList);
	
	int i =0;
	struct URLRecordList *iterate;
	
	while (iterator.next(word, rec))
	{
		outFile.open (wordFileName);
		iterate = rec;
		//printf("iterate:%d word: %s\n", i, word);
		outFile<<word;
		printf("%s", word);
		do{
			
			outFile<<" "<<iterate -> _urlRecordIndex;
			printf("   index: %d\n", iterate -> _urlRecordIndex);
			iterate = iterate -> _next;
			
		}while (iterate != NULL);
		outFile<<"\n\n";	
		printf("\n\n");
		i++;
		outFile.close();
	}
	
	/*
	for (int i = 0; i<_maxUrls; i++)
	{
		printf("word: %s\n", _wordToURLRecordList -> _buckets[i] -> _key);
	}
	*/
	
	return;
}

void WebCrawler::trim(char * s) //trims whitespace before, after, and in a character string
{
  int n = 0;
  int i = 0;
  //char *x = (char*)malloc(sizeof(strlen(s))+1);
  while(s[n] != '\0'){
    while(s[n] == ' ' || s[n] == '\t' || s[n] == '\n')
      n++;
    while(s[n] != ' ' && s[n] != '\t' && s[n] != '\n' && s[n] != '\0'){
      //printf("%c",s[n]);
      s[i++] = s[n++];
    }
    if(s[n] == '\0')
    {
      s[i]=s[n];
      break;
    }
    s[i++]=' ';
    //printf(" ",s);
    n++;
  }
  s[i] = '\0';
  if(s[i-1] == ' ' || s[i-1] == '\t' ||  s[i-1] == '\n')
    s[i-1] = '\0';
}

void WebCrawler::description(char * html)
{

  char* ctag; //This is the compiled regular expression created with regcmp
  //that is passed to regex
  char* ctag2;//This is the compiled regular expression created with regcmp
  //and passed to regex. Used to demark </script>
  
  char* ctagp; //corresponding pointers used to mark locations of tags
  char* ctag2p;//
  
  if ((ctag = regcmp("<[Hh][tT][mM][lL]", NULL)) == NULL)
  {
	  printf("bad malloc\n");
	  exit(0);
  }
  ctag2 = regcmp(">",NULL);
  if((ctagp = regex(ctag,html)) != NULL){
    ctagp = regex(ctag2,ctagp);
    html[0] = '\0';
    strcat(html, ctagp);
  }

  //remove scripts from html
  ctag = regcmp("<[Ss][Cc][Rr][Ii][pP][Tt]", NULL); //[ -=?-~]*>
  ctag2 = regcmp("</[Ss][Cc][Rr][Ii][Pp][Tt]>", NULL);
  //printf("%s\n\n\n",html);
  //printf("length = %d",strlen(html));
  while(1){
    if( (ctagp = regex(ctag,html)) == NULL)
      break; //no script tag found
    ctagp = __loc1; //begging of <scipt ...>
    if( (ctag2p = regex(ctag2,ctagp)) == NULL) //end of </script>
      break;
    //html needs a null character where ctagp is pointing to and should be
    //concatenated with ctag2p to remove script
      //printf("right number is %d\n", __loc1-html);
      while(ctagp != ctag2p){
	ctagp[0] = ' ';
	ctagp++;
      }
    }//scripts removed, now remove comments
  free(ctag);
  free(ctag2);
  ctag = regcmp("<!--", NULL); //[ -=?-~]*>
  ctag2 = regcmp("-->", NULL);
  while(1){
    if( (ctagp = regex(ctag,html)) == NULL)
      break; //no comment tag found
    ctagp = __loc1; //beginning of comment <!--
    if( (ctag2p = regex(ctag2,ctagp)) == NULL) //end of comment -->
      break;
    //html needs a null character where ctagp is pointing to and should be
    //concatenated with ctag2p to remove script
      while(ctagp != ctag2p){
	ctagp[0] = ' ';
	ctagp++;
      }
  }//comments removed, remove other tags
  free(ctag);
  free(ctag2);
  ctag = regcmp("<", NULL); //[ -=?-~]*>
  ctag2 = regcmp(">", NULL);
  while(1){
    if( (ctagp = regex(ctag,html)) == NULL)
      break;
    ctagp = __loc1;
    if( (ctag2p = regex(ctag2,ctagp)) == NULL) //find next > starting at <
      break;
      while(ctagp != ctag2p){
	ctagp[0] = ' ';
	ctagp++;
      }
  }//tags removed, now remove anything from & to ;
  free(ctag);
  free(ctag2);
  ctag = regcmp("&", NULL);
  ctag2 = regcmp(";", NULL);
  while(1){
    if( (ctagp = regex(ctag,html)) == NULL)
      break;
    ctagp = __loc1;
    if( (ctag2p = regex(ctag2,ctagp)) == NULL) //find next ; starting at &
      break;
      while(ctagp != ctag2p){
	ctagp[0] = ' ';
	ctagp++;
      }
  }//find punctuation that will be cut
  //for "'" character (i.e can't or Joe's): you can ignore every letter 
  //after "'" and keep the part of the word before 
  free(ctag);
  free(ctag2);
  ctag = regcmp("'", NULL);
  ctag2 = regcmp(" ", NULL);
  while(1){
    if( (ctagp = regex(ctag,html)) == NULL)
      break;
    ctagp = __loc1;
    if( (ctag2p = regex(ctag2,ctagp)) == NULL)
      break;
      while(ctagp != ctag2p){
	ctagp[0] = ' ';
	ctagp++;
      }
  }//remove punctuation
  free(ctag);
  free(ctag2);

 int y = 0;
    while(html[y] != '\0'){
      //if( (html[i] > '!' && html[i]<'@') || (html[i] > '[' && html[i]<'`') || (html[i] > '{' && html[i]<']' || html[i] ==  )
      if( (html[y]>='a' && html[y]<='z') || (html[y]>='A' && html[y]<='Z') || html[y] ==' ' ||  html[y] =='\n' ||  html[y] == '\t')
	y++;
      else{
	html[y] = ' ';
	y++;
      }
    } 
  
  
  trim(html);
   
  int i = 0, spaceCount = 0;
  
  // printf("%s",html);
  // strcpy(desc, html);
	    
}
void WebCrawler::hyperlinks( char* html)
{
 char* ctag;
 char* ctag2;
 char* ctagp;
 char* ctag2p;
 char* urls = new char[500000];
 if (urls == NULL)
	 exit(1);
 char* begin = urls;

 printf("html: %s\n", html);
 

 char * base = strdup( _urlArray[_headURL]._url);

 printf("base: %s\n", base);
       if(strncmp(base + strlen(base) - 5,".html",5)==0  || strncmp(base + strlen(base) - 5,".htm",4)==0 ||  strncmp(base + strlen(base) - 5,".shtml",4)==0 )
	while(base[strlen(base)-1] != '/')
	  base[strlen(base)-1] = '\0';
      if(base[strlen(base)-1] == '/')
	base[strlen(base)-1] = '\0';
      ctagp = html;

      ctag = regcmp("<[Aa] [hH][Rr][eE][fF]=\"", NULL);
      ctag2 = regcmp("\"", NULL);
      while(1){
	if( (ctagp = regex(ctag,ctagp)) == NULL)
	  break;
	if( (ctag2p = regex(ctag2,ctagp)) == NULL)
	  break;
	ctag2p--; //1 less that "
	if(strncmp(ctagp,"mailto:",7)==0 || ctagp[0] == '#'){
	  continue;
	}
	else if( (strncmp(ctagp,"http",4)!=0) && (strncmp(ctagp,"ftp://",6)!=0)) {
	  if(ctagp[0] == '/'){
	    //get to the root of the base tag
	    int index = 0;
	    int slashes = 0;
	    while(slashes!=3){
	      urls[index++] = base[index++];
	      if(base[index] == '/' || base[index] == '\0')
		slashes++;
	    }
	    urls += index;	      
	  }
	  else if(strncmp(ctagp,"../",3) == 0){ //move back x../ directories in base
	    char * newbase = strdup(base);
	    //printf("newbase\n");
	    while(strncmp(ctagp,"../",3) == 0){
	      while(newbase[strlen(newbase)-1] != '/'){
		newbase[strlen(newbase)-1] = '\0';		
	      }
	      if(newbase[strlen(newbase)-1] == '/')
		newbase[strlen(newbase)-1] = '\0';
	      ctagp +=3;
	    }
	    strncpy(urls,newbase,strlen(newbase));
	    urls += strlen(newbase);
	    urls[0] = '/';
	    urls++;
	    free(newbase);
	  }
	  else{
	    if((strncmp(ctagp,"https",5)==0) || (strncmp(ctagp,"ftp://",6)==0))
		  continue;
	    strncpy(urls,base,strlen(base));
	    urls += strlen(base);
	    urls[0] = '/';
	    urls++;
	  }
	}
 
       strncpy(urls,ctagp,ctag2p-ctagp);
       urls +=ctag2p-ctagp;
       urls[0] = '\n';
       urls++;
     }
     urls[0] = '\0';
     urls = begin;

     printf("urls:%s\n", urls);
 strcpy(html, urls);
 delete(urls);
 free(base);
 free(ctag);
 free(ctag2);
}


int main (int argc, char **argv)
{
  bool utag = false;
  int maxurls = 1000;
  int n = 1;
  argv++;
  if ( !strcmp(*argv,"-u") ) {
    utag = true;
    argv++;
    n++;
    maxurls = atoi(*argv);
    argv++;
    n++;
  }
  n = argc - n;

  WebCrawler wc = WebCrawler(maxurls, n, (const char**)argv);
  wc.crawl();  
  return 0;
}
