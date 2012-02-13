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
// Definitions to use openhttp
//

enum {
  MaxURLLength = 512,
  MaxLineLength = 1024
};

// connect to an HTTP server and fetch the document.
// Parameters:
//   url: points to the URL to fetch.
//   contentType: points to a string where the content type is returned
//                It should be of size MaxLineLength
// Result:
//   It returns a stream that can be used with functions like fgets()
//   The stream should be closed once it is no longer used.

FILE * openhttp( const char * url, char * contentType );

