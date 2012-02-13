/*
  Copyright (c) 2005
  Gustavo Rodriguez-Rivera
  All rights reserved. 

This work was developed by the author(s) at Purdue University
during 2005.
 
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

#include <stdio.h>

// Class that represents a simple HTTP server to test your search
// engine.
// 
// IMPORTANT: Do not use in a production environment until you fully
// test it. Kill the server when not in use. Do not leave it running.
//
class MiniHTTPD {
  int _port;
 public:
  // Create a new instance of the HTTP server
  MiniHTTPD( int port );

  // When a request arrives this method is called.
  // The argument "out" is a FILE * descriptor that will be used
  // to send the repsonse using formatted output with fprintf.
  // The argument "documentRequested" contains the document requested.
  // You need to redefine "dispatch" in a subclass
  // to do something useful. 
  virtual void dispatch( FILE * out, const char * documentRequested );

  // Wait for incoming requests. The method never returns
  void run();
};

