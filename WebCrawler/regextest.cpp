#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>

void trim(char *s);

main()
{
  char* type;
  char * compiled;
  char *tagstartc;//this is the compiled regular expression to be passed into 
  char *tagend;
  char* content = "Script Remove Test <script defer> delete this script </script> Content Type: \"text/html\"\n";
  char* content2 = "New Line of stuff to be concatenated";
  printf("The content equals %s", content);
  if( (compiled = regcmp("<script>", NULL)) == NULL) //[ -=?-~]*>
    printf("Failure\n");
  //printf("The content equals %s", compiled);
  if((type = regex(compiled,content))!=NULL){
    printf("Type points to %s\n", type);
    printf("*__loc1 points to %s\n", __loc1);
  }
}

void trim(char *s) //trims whitespace before, after, and in a character string
{
  int n;
  int i;
  int aSpace = 0;
  
  for (n = 0; n<=strlen(s)-1; n++) //removes whitespace before
  {
    if (s[n] != ' ' && s[n] != '\t' && s[n] != '\n')
    {
      for(i= 0; n<=strlen(s);n++,i++)
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
}
