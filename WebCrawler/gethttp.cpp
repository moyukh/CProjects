/*
  Copyright (c) 2004
  Gustavo Rodriguez-Rivera
  All rights reserved. 

This work was developed by the author(s) at Purdue University
during 2004.
 
Redistribution and use in source and binary forms are permitted provided that
this entire copyright notice is duplicated in all such copies.  No charge,
other than an "at-cost" distribution fee, may be charged for copies,
derivations, or distributions of this material without the express written
consent of the copyright holders. Neither the name of the University, nor the
name of the author may be used to endorse or promote products derived from
this material without specific prior written permission.

THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR ANY PARTICULAR PURPOSE.
*/

//
// gethttp:
//   Example program that shows how to use openttp.
//   It gets the URL passed as argument
//
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include "openhttp.h"

bool ttag = false; // turns true when -t
bool atag = false; // turns true when -a
char* ctag; //This is the compiled regular expression created with regcmp
            //that is passed to regex
char* ctag2;//This is the compiled regular expression created with regcmp
             //and passed to regex.

char* ctagp; //corresponding pointers used to mark locations of tags
char* ctag2p;

char* html = new char[5000000]; //This is the html document to be parsed
char* urls = new char[5000000];
void trim(char *s);

void
printUsage()
{
  fprintf( stderr, "  Usage: gethttp [-h|-t|-a] url\n");
  fprintf( stderr, "  Example: gethttp http://www.cs.purdue.edu\n");
}

int
main( int argc, char ** argv )
{
  // Skip command
  argv++;
  // Process the arguments
  if ( !strcmp(*argv,"-h") ) {
    printUsage();
    exit(1);
  }
  
  if ( !strcmp(*argv,"-t") ) {
    ttag = true;
    argv++; //argv[0] should be the html. The following NULL test still applies
  }
  if ( !strcmp(*argv,"-a") ) {
    atag = true;
    argv++; //argv[0] should be the html. The following NULL test still applies
  }

  if ( *argv == NULL ) {
    // Print usage also if no URL after the arguments
    printUsage();
    exit(1);
  }

  // Open URL
  char contentType[ MaxLineLength ];
  char * url = *argv;


  FILE * f = openhttp( url, contentType );
  if ( f == NULL ) {
    exit(1);
  }

  if(ttag == false && atag == false){ //no -t tag
    // Print the content type
    printf( "Content Type: \"%s\"\n", contentType );

    // Print to stdout line by line
    char line[ MaxLineLength ];
    while ( fgets( line,MaxLineLength, f ) ) {
      fputs( line, stdout );
    }
  }
  else{//-t or -a argument
    if(strcmp(contentType,"text/html") != 0){
      printf("Error: document is not text/html\n");
      exit(0);
    }
    printf( "Content Type: \"%s\"\n", contentType );
    char line[ MaxLineLength ];
    while ( fgets( line,MaxLineLength, f ) ) {
      strcat(html, line);
    }

    ctagp = html;
    if(atag == true){

      char* begin = urls;
      char * base = strdup(url);
      //printf("base = %s\n",base + strlen(base) - 6);      
      if(strncmp(base + strlen(base) - 5,".html",5)==0  || strncmp(base + strlen(base) - 5,".htm",4)==0 ||  strncmp(base + strlen(base) - 5,".shtml",4)==0 )
	while(base[strlen(base)-1] != '/')
	  base[strlen(base)-1] = '\0';
      if(base[strlen(base)-1] == '/')
	base[strlen(base)-1] = '\0';


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

      printf("%s", begin);
      delete(html);
      delete(begin);
      fclose( f );  
      exit(0);
    }


    if ((ctag = regcmp("<[Hh][tT][Mm][Ll]", NULL))==NULL){
      printf("invalid expression exitting\n");
      exit(0);
    }
    ctag = regcmp(">",NULL);
    ctagp = regex(ctag,html);
    ctag2p = regex(ctag,html);
    html[0] = NULL;
    strcat(html, ctagp);

    //remove scripts from html
    ctag = regcmp("<script", NULL); //[ -=?-~]*>
    ctag2 = regcmp("</script>", NULL);
    //printf("%s\n\n\n",html);
    //printf("length = %d",strlen(html));
    ctagp = html;
    while(1){
      if( (ctagp = regex(ctag,ctagp)) == NULL)
	break; //no script tag found
      ctagp = __loc1; //begging of <scipt ...>
      if( (ctag2p = regex(ctag2,ctagp)) == NULL) //end of </script>
	break;
      //html needs a null character where ctagp is pointing to and should be
      //concatenated with ctag2p to remove script
      //printf("right number is %d\n", __loc1-html);
      //html[ctagp-html] = ' ';
      //html[ctagp-html+1] = NULL;
      //strcat(html, ctag2p);
      //new function
      while(ctagp != ctag2p){
	ctagp[0] = ' ';
	ctagp++;
      }

    }//scripts removed, now remove comments
    free(ctag);
    free(ctag2);
    ctag = regcmp("<!--", NULL); //[ -=?-~]*>
    ctag2 = regcmp("-->", NULL);
    ctagp = html;
    while(1){
      if( (ctagp = regex(ctag,ctagp)) == NULL)
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
    ctagp = html;
    while(1){
      if( (ctagp = regex(ctag,ctagp)) == NULL)
	break;
      ctagp = __loc1;
      if( (ctag2p = regex(ctag2,ctagp)) == NULL) //find next > starting at <
	break;
      while(ctagp != ctag2p){
	ctagp[0] = ' ';
	ctagp++;
      }
    }
    //tags removed, now remove anything from & to ;
    free(ctag);
    free(ctag2);
    ctag = regcmp("&", NULL);
    ctag2 = regcmp(";", NULL);
    ctagp = html;
    while(1){
      if( (ctagp = regex(ctag,ctagp)) == NULL)
	break;
      ctagp = __loc1;
      if( (ctag2p = regex(ctag2,ctagp)) == NULL) //find next ; starting at &
	break;
      while(ctagp != ctag2p){
	ctagp[0] = ' ';
	ctagp++;
      }
    }
    //find punctuation that will be cut
    //for "'" character (i.e can't or Joe's): you can ignore every letter 
    //after "'" and keep the part of the word before 
    free(ctag);
    free(ctag2);
    ctag = regcmp("'", NULL);
    ctag2 = regcmp(" ", NULL);
    ctagp = html;
    while(1){
      if( (ctagp = regex(ctag,ctagp)) == NULL)
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
    
    //ctag = regcmp("[!-@[-`{-~]", NULL);
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
    //while(1){
    //if( (ctagp = regex(ctag,html)) == NULL)
    //break;
    //html[__loc1-html] = ' ';
    //printf("i = %d\n",i++);
    //}
    trim(html);
    //printf("length = %d\n",strlen(html));
    printf("%s\n",html);
  }
  delete(html);
  delete(urls);
  fclose( f );  
  exit(0);
}

/*
void trim(char *s) //trims whitespace before, after, and in a character string
{
  int n = 0;
  int i = 0;
  int aSpace = 0;
  
  for (n = 0; n<=strlen(s)-1; n++) //removes whitespace before
  {
    if (s[n] != ' ' && s[n] != '\t' && s[n] != '\n')
    {
      for(i= 0; n < strlen(s);n++,i++)
	s[i]=s[n];
      break;
    }
  }
  for (n = strlen(s) - 1; n >=0; n--) //then removes whitespace after
    if (s[n] != ' ' && s[n] != '\t' && s[n] != '\n')
      break;
  s[n+1] = '\0';
  for (n = 0, i = 0; n<=strlen(s)-1; n++, i++) //removes whitespace in between
  {
    if (s[n] == ' ' || s[n] == '\t' || s[n] == '\n')
    {
      s[i] = ' ';
      i++;
      while(s[n] == ' ' || s[n] == '\t' || s[n] == '\n')
	n++;
    }
    s[i] = s[n];
  }
  s[i]= '\0';
  //printf("i = %d",i);
}
*/

void trim(char *s) //trims whitespace before, after, and in a character string
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
  
	  
