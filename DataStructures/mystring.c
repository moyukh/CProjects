#include<stdlib.h>
#include<stdio.h>
/*
 * Implement the mystrcpy, mystrcmp, mystrstr, mystrdup procedures
 * IMPORTANT: Do not use string functions.
 */

/*
 * Copies string "src" to "dest"
 * Returns a pointer to "dest".
 */
char *
mystrcpy( char * dest, const char * src )
{
	
	char* temp = dest; //temp pts to beginning of dest
	while (*src != '\0')
	{
		*dest++ = *src++; //increment and equate at the same time
				  //I thought this was really cool
	}
	*dest = '\0';
	return temp; //return beginning of dest
}

/*
 * Compares two strings alphabetically. It returns:
 *
 * -1 if a < b
 *  0 if a == b
 *  1 if a > b
 */
int
mystrcmp( const char * a, const char * b )
{
  /* Complete procedure */
  	while(1)
	{
		if (*a < *b)//comparison
			return -1;
		else if (*a > *b)
			return 1;
		else
		{
			if ((*a == '\0') || (*b == '\0')) //end of string, gotta be =
				return 0;
			*a++;//increment
			*b++;
		}
	}
}

/*
 * strstr() locates the  first  occurrence  of  the  string  b
 * (excluding  the  terminating  null  character) in string a.
 * strstr() returns a pointer to the located string, or a  null
 * pointer  if  the  string  is  not  found.  If b points to a
 * string with zero length (that is, the string ""), the  func-
 * tion returns a.
 */

char *
mystrstr(const char *a, const char *b)
{
	const char* tmp = a;
	int found = 0;
	int i = 0, j=0;
  /* Complete procedure */
 while (*a != '\0')
 {
	 while (*(a+i) == *(b+i))//check if b is in current position of a
	 {
		//printf("%c = %c\n", *(a+i), *(b+i));
		i++;
		if (*(b+i)=='\0') //the end of b is reached
		{		  //whole word found
			return (char *)a;
		}
		
	 }
	
	 *a++; //next character to compare starts with
	 i=0;
 }
	  
  return NULL; //word not found
}

/*
 * strdup() returns a pointer to a new string that is a  dupli-
 * cate  of the string pointed to by s1.  The space for the new
 * string is obtained using malloc(3C).  If the new string can-
 * not be created, a null pointer is returned.
 */
char *
mystrdup(const char *s1)
{
  char* newString;
  int len = 0;
  
  while (*(s1 + len) != '\0') //find string length
  {
	  len++;
  }
  
  newString = (char*)malloc(len); //allocate new string space
  
  return mystrcpy(newString, s1); //copy into new string
}

