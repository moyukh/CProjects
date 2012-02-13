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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "minihttpd.h"

// Implementation of the simple HTTP Server
// Create an instance

MiniHTTPD::MiniHTTPD( int port )
{
  _port = port;
}

// Wait for incoming requests.
// This method never returns.
void
MiniHTTPD::run()
{
  // Structure to hold server's address
  struct sockaddr_in serverAddress; 
  
  // Clear serverAddress
  memset((char *)&serverAddress,0,sizeof(serverAddress));

  // Set family to Internet
  serverAddress.sin_family = AF_INET;

  // Use any address available in the host
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  
  // Assign port 
  if (_port > 0) {
    serverAddress.sin_port = htons((u_short) _port);
  }
  else {
    fprintf(stderr,"MiniHTTPD: bad port number %d\n", _port);
    exit(1);
  }

  // Get TCP transport protocol entry
  struct protoent *ptrp = getprotobyname("tcp");
  if ( ptrp == NULL ) {
    fprintf(stderr, "MiniHTTPD: cannot map \"tcp\" to protocol number");
    perror("getprotobyname");
    exit(1);
  }

  // Create a tcp socket
  int masterSocket = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
  if (masterSocket < 0) {
    fprintf(stderr, "MiniHTTPD: socket creation failed\n");
    perror("socket");
    exit(1);
  }

  // Set socket options to reuse port. Otherwise we will
  // have to wait about 2 minutes before reusing the sae port number
  int optval = 1; 
  int err = setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, 
		       (char *) &optval, sizeof( int ) );

  // Bind address structure to this socket
  if (bind(masterSocket, (struct sockaddr *)&serverAddress,
	   sizeof(serverAddress)) < 0) {
    fprintf(stderr,"MiniHTTPD: bind failed\n");
    perror("bind");
    exit(1);
  }

  // Specify size of request queue and put socket in listen mode
  int QueueLength = 20;
  if (listen(masterSocket, QueueLength) < 0) {
    fprintf(stderr,"MiniHTTPD: listen failed\n");
    perror("listen");
    exit(1);
  }

  // Get host name
  const int MaxHostName = 256;
  char hostName[MAXHOSTNAMELEN+1];
  if (gethostname(hostName, MAXHOSTNAMELEN) != 0) {
    strcpy(hostName, "localhost");
  }
  
  fprintf( stderr, "MiniHTTPD: Listening at \"http://%s:%d\"\n",
	   hostName, _port);
  
  // Main server loop - accept and handle requests forever
  while (1) {

    // structure to hold client's address
    struct sockaddr_in clientAddress;

    // Wait until the next request arrives
    socklen_t alen = sizeof(clientAddress);
    int slaveSocket = accept(masterSocket,
			     (struct sockaddr *)&clientAddress, &alen);
    if (slaveSocket < 0) {
      fprintf(stderr, "MiniHTTPD: accept failed\n");
      exit(1);
    }

    // Make the input slave socket a buffered file

    // Create a duplicate of the file descriptor to use for the
    // input streamed file
    int fdindup = dup(slaveSocket);
    if (fdindup < 0) {
      fprintf(stderr, "MiniHTTPD: Cannot dup slaveSocket\n");
      perror("fdopen");
      close(slaveSocket);
      continue;
    }

    // Create FILE* stream file from fdindup
    FILE * fin = fdopen( fdindup, "r");
    if (fin == NULL) {
      fprintf(stderr, "MiniHTTPD: Cannot create FILE *\n");
      perror("fdopen");
      close(slaveSocket);
      continue;
    }

    // Allocate space for the request line
    const int MaxLine = 1000;
    char line[ MaxLine + 1 ];
    int i = 0;

    // Read the entire request
    int chend[4];
    int isFirstLine = 1;
    while ((chend[0] = fgetc(fin)) != EOF) {

      // Read until the cr-lf-cr-lf sequence is received marking
      // the end of the request
      if (chend[3]=='\r' && chend[2]=='\n' && chend[1]=='\r' && chend[0]=='\n') {
	break;
      }

      // Check if first line has ended
      if (chend[0] == '\r' || chend[0] == '\n') {
	isFirstLine = 0;
      }
      
      // Get first line. Discard the rest
      if ( isFirstLine && i < MaxLine) {
	line[i] = chend[0];
	i++;
      }

      // Shift previous characters
      chend[3]=chend[2];
      chend[2]=chend[1];
      chend[1]=chend[0];
    }

    // Check if EOF was reached prematurely
    if (chend[0] == EOF) {
      fprintf(stderr, "MiniHTTPD: Incomplete request\n");
      fclose(fin);
      close(slaveSocket);
      continue;
    }

    // Add null char at the end of line
    line[i] = 0;
    
    fprintf(stderr, "MiniHTTPD: Request: %s\n", line);

    // Make the output slave socket a buffered file
    FILE * fout = fdopen( slaveSocket, "w");
    if (fout == NULL) {
      fprintf(stderr, "MiniHTTPD: Cannot create FILE *\n");
      perror("fdopen");
      fclose(fin);
      close(slaveSocket);
      continue;
    }

    // Send HTTP header
    fprintf( fout, "HTTP/1.1 200 OK\r\n");
    fprintf( fout, "Content-type: text/html\r\n\r\n");

    // Extract document requested
    char *p = line;

    // Skip command section
    while (*p != 0 && *p !=' ') p++;

    // Skip white spaces
    while (*p != 0 && *p ==' ') p++;

    const char * documentRequested = p;
    // Skip until end of ducument
    while (*p != 0 && *p !=' ') p++;
    *p = 0;

    // Dispatch request
    dispatch( fout, documentRequested );

    // Flush the output
    fflush(fout);

    // Close fout first. This will close the file descriptor as well.
    fclose(fout);
    fclose(fin);
  }
  
}

// Default dispatch implementation
void
MiniHTTPD::dispatch( FILE * fout, const char * documentRequested )
{
  fprintf( stderr, "MiniHTTPD: Document requested:\"%s\"\n", documentRequested);
  fprintf( fout, "<TITLE>Welcome</TITLE>\n");
  fprintf( fout, "<H1><CENTER>MiniHTTPD</CENTER></H1>\n");
  fprintf( fout, "<H2><CENTER>\n");
  fprintf( fout, "You need to subclass MiniHTTPD and redefine dispatch()\n");
  fprintf( fout, "in the subclass to do something more useful than printing\n");
  fprintf( fout, "this message.</H2></CENTER>\n");
  fprintf( fout, "Document Requested: \"%s\"\n", documentRequested);
}

#if defined(STANDALONE)
// Used for testing of MiniHTTPD
int
main()
{
  MiniHTTPD httpd(8080);
  httpd.run();
}
#endif
