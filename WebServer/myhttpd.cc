const char * usage =
"                                                               \n"
"myhttpd:                                                \n"
"                                                               \n"
"To use it in one window type:                                  \n"
"                                                               \n"
"   myhttpd [-f|-t|-p] [<port>]                                       \n"
"                                                               \n"
"Where 1024 < port < 65536.             \n"
"                                                               \n"
"In another window type:                                       \n"
"                                                               \n"
"   telnet <host> <port>                                        \n"
"                                                               \n"
"where <host> is the name of the machine where daytime-server  \n"
"is running. <port> is the port number you used when you run   \n"
"myhttpd.                                               \n"
  "                                                               \n"
"or open a browser and connect to the server by inputing the  \n"
"host name after http:// followed by : port then / file \n\n";

#define __USE_MISC

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <dlfcn.h>
#include <link.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <dirent.h>
#include <semaphore.h>
#include <ctype.h>
#include <ext/stdio_filebuf.h>
#include <iostream>
#include <ostream>
#include <arpa/inet.h>

typedef void (*httprunfunc)(int ssock, char *queryString);
using namespace std;
int QueueLength = 5;

void *processTimeRequestWrapper( int * );
char * recurseSlash(char *, int);
void ignore(int sig);
void zombie(int sig);
void processTimeRequest(int socket);
void threadServer(int *socket);
struct module * checkLoaded(char *);

pthread_mutex_t mutexMe;
time_t timeIt;

//Port number the server is on
int portNum;

//Number of requests the server has handled
int timesCalled;

//Url taking longest time and time taken
char * longURL;
double longTime;

//Url taking shortest time and time  taken
char *shortURL;
double shortTime;

//Remote Address and remote host
char *remoteAddy;
char *remoteHost;

struct module *modulesLoaded;
int numModulesLoaded;

typedef struct module {
  char *filename;
  void *library;
} mods;

extern "C" void igSig(int sig){
  printf("Handled broken pipe");
}

extern "C" void ignoreChild(int sig){
  int status;
  struct rusage usage;
  int pid;
  while(pid = wait4(pid, &status, WNOHANG, &usage) > 0);
}
int main( int argc, char ** argv )
{
  longTime = -1;
  shortTime = -1;
  modulesLoaded = (module *)malloc(sizeof(module)*1024);
  numModulesLoaded = 0;
  timeIt = time(NULL);
  int pid; //used for execvp
  timesCalled = 0;

  //Handle the broken pipe if it occurs
  struct sigaction handleBrokenPipe;
  handleBrokenPipe.sa_handler = igSig;
  sigemptyset(&handleBrokenPipe.sa_mask);
  handleBrokenPipe.sa_flags = SA_RESTART;
  sigaction(SIGPIPE, &handleBrokenPipe, NULL);

  //Used to initialize mutex 
  pthread_mutex_init(&mutexMe, NULL);

  
  struct sigaction signalAction; //We need this to kill zombie processes but it causes a Interrupted System Call when using threads - signalAction ignore might work but SA_RESTART kills the server (possibly WNOHANG might fix)
    signalAction.sa_handler = ignoreChild;
    sigemptyset(&signalAction.sa_mask);
    signalAction.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &signalAction, NULL);

  // Print usage if not enough arguments

  if ( argc > 3) {
    fprintf( stderr, "%s", usage );
    exit( -1 );
  }
  if(argv[1] != NULL){
    if(argv[1][0] == '-' && argv[1][1] != 'f' && argv[1][1] != 't' && argv[1][1] != 'p'){
      fprintf( stderr, "%s", usage );
      exit( -1 );
    }
    else if(argv[1][0] == '-' && strlen(argv[1]) > 2){
      fprintf( stderr, "%s", usage );
      exit( -1 );
    }
    else if(argv[1][0] != '-'){
      for(int checkIt = 0; argv[1][checkIt] != '\0'; checkIt++){
        if(!isdigit(argv[1][checkIt])){
          fprintf( stderr, "%s", usage );
          exit( -1 );
        }
      }
    }
    else if(argc == 3){
      for(int checkIt = 0; argv[2][checkIt] != '\0'; checkIt++){
        if(!isdigit(argv[2][checkIt])){
          fprintf( stderr, "%s", usage );
          exit( -1 );
        }
      }
    }
  }

  // Get the mode & port from the arguments
  char *mode = NULL;
  int port;
  if(argc == 3){
    mode = strdup(argv[1]);
    port = atoi( argv[2] );
  }
  else if(argc == 2){
    if(argv[1][0] == '-'){
      mode = strdup(argv[1]);
      port = 9000;
    }
    else{
      mode = strdup("none");
      port = atoi(argv[1]);
    }
  }
  else if(argc == 1){
    mode = strdup("none");
    port = 9000;
  }

  if(port < 1025 || port > 65536){
    fprintf( stderr, "%s", usage );
    exit( -1 );
  }

  // Set the IP address and port for this server
  struct sockaddr_in serverIPAddress; 
  memset( &serverIPAddress, 0, sizeof(serverIPAddress) );
  serverIPAddress.sin_family = AF_INET;
  serverIPAddress.sin_addr.s_addr = INADDR_ANY;
  serverIPAddress.sin_port = htons((u_short) port);

  portNum = port;

  // Allocate a socket
  int masterSocket =  socket(PF_INET, SOCK_STREAM, 0);
  if ( masterSocket < 0) {
    perror("socket");
    exit( -1 );
  }

  // Set socket options to reuse port. Otherwise we will
  // have to wait about 2 minutes before reusing the same port number
  int optval = 1; 
  int err = setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, 
      (char *) &optval, sizeof( int ) );

  // Bind the socket to the IP address and port
  int error = bind( masterSocket,
      (struct sockaddr *)&serverIPAddress,
      sizeof(serverIPAddress) );
  if ( error ) {
    perror("bind");
    exit( -1 );
  }

  // Put socket in listening mode and set the 
  // size of the queue of unprocessed connections
  error = listen( masterSocket, QueueLength);
  if ( error ) {
    perror("listen");
    exit( -1 );
  }

  while ( 1 ) 
  {
    if(strcmp(mode, "-f") == 0)
    {
      struct sockaddr_in clientIPAddress;
      int alen = sizeof( clientIPAddress );
      int slaveSocket = accept( masterSocket, (struct sockaddr *)&clientIPAddress, (socklen_t*)&alen);
      struct hostent *addrGet = gethostbyaddr((char *)&clientIPAddress.sin_addr, alen, AF_INET);
      if(addrGet != NULL){
        remoteAddy = addrGet->h_name;
      }
      remoteHost = inet_ntoa(clientIPAddress.sin_addr);
/*
      if ( slaveSocket < 0 ) {
        perror( "accept" );
        exit( -1 );
      }

      pid = fork();
      if(pid == 0)
      {
        processTimeRequest(slaveSocket);
        perror("processTimeRequest");
        exit(1);
      }
      else if(pid < 0)
      {
        perror("fork");
        exit(2);
      }
      close(slaveSocket);*/
      int rt;
      if (slaveSocket >= 0)
      {
	      	rt = fork();
	     
	     	if(rt == 0)
		{
			processTimeRequest(slaveSocket);
			exit(0);
		} 
		close(slaveSocket);
     }
     
    }
    else if(strcmp(mode, "-t") == 0)
    {
      struct sockaddr_in clientIPAddress;
      int alen = sizeof( clientIPAddress );
      pthread_mutex_lock(&mutexMe);
      int slaveSocket = accept( masterSocket, (struct sockaddr *)&clientIPAddress, (socklen_t*)&alen);
      struct hostent *addrGet = gethostbyaddr((char *)&clientIPAddress.sin_addr, alen, AF_INET);
      if(addrGet != NULL){
        remoteAddy = addrGet->h_name;
      }
      remoteHost = inet_ntoa(clientIPAddress.sin_addr);
      pthread_mutex_unlock(&mutexMe);

      if ( slaveSocket < 0 ) {
        perror( "accept" );
        exit( -1 );
      }

      pthread_t t1;
      pthread_attr_t attr;
      pthread_attr_init(&attr);
      pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
      pthread_create(&t1, &attr, (void * (*)(void *))processTimeRequestWrapper,&slaveSocket);
      pthread_join(t1, NULL);
    }
    else if(strcmp(mode, "-p") == 0){
      break;
    }
    else{
      struct sockaddr_in clientIPAddress;
      int alen = sizeof( clientIPAddress );
      int slaveSocket = accept( masterSocket, (struct sockaddr *)&clientIPAddress, (socklen_t*)&alen);
      remoteHost = inet_ntoa(clientIPAddress.sin_addr);
      struct hostent *addrGet = gethostbyaddr((char *)&clientIPAddress.sin_addr, alen, AF_INET);
      if(addrGet != NULL){
        remoteAddy = addrGet->h_name;
      }

      if ( slaveSocket < 0 ) {
        perror( "accept" );
        exit( -1 );
      }

      processTimeRequest(slaveSocket);
      close(slaveSocket);
    }

  }  
  if(strcmp(mode, "-p") == 0)
  {
    pthread_t t1[5];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    for(int tCount = 0; tCount < 5; tCount++){
      pthread_create(&t1[tCount], &attr, (void * (*)(void *))threadServer, &masterSocket);
    }
    for(int tCount = 0; tCount < 5; tCount++){
      pthread_join(t1[tCount], NULL);
    }
  }
}

void *processTimeRequestWrapper( int * fdp )
{
  int fd = *fdp;
  processTimeRequest( fd );
}

void processTimeRequest( int fd )
{ 
  //Time the request
  /*
  timer_t *timeReq;
  struct itimerspec value;
  struct itimerspec ovalue;
  timer_create(CLOCK_REALTIME, NULL, timeReq);
  value.it_interval.tv_sec = 0;
  value.it_interval.tv_nsec = 0;
  value.it_value.tv_sec = 1000000;
  value.it_value.tv_nsec = 0;
  timer_settime(*timeReq, 0, &value, NULL);
  timer_gettime(*timeReq, &ovalue);
*/

  time_t start,end;
  
  time (&start);
  // Buffer used to store the name received from the client
  const int MaxName = 1024;
  char name[ MaxName + 1 ];
  int nameLength = 0;
  int n;
  char * curDir = (char *)calloc(sizeof(char), MaxName);

  //Increment number of requests
  timesCalled++;

  //Remember the current working directory
  //for restoring purposes
  getcwd(curDir, MaxName);

  // Currently character read
  unsigned char newChar;

  // Last character read
  unsigned char lastChar = 0;

  //Record the last 4
  char lastfour[4];
  int a = 0;

  // The client should send GET <space> /file HTTP/1.1
  while(nameLength < MaxName && (n = read(fd, &newChar, sizeof(newChar)) > 0)){
    name[nameLength] = newChar;
    nameLength++;
    if(a < 4){
      lastfour[a] = newChar;
      a++;
    }
    else{
      for(int b = 0; b < 3; b++){
        lastfour[b] = lastfour[b+1];
      }
      lastfour[3] = newChar;
    }
    if(lastfour[0] == '\015' && lastfour[1] == '\012' && lastfour[2] == '\015' && lastfour[3] == '\012'){
      break;
    }
  }

  // Add null character at the end of the string
  name[ nameLength ] = '\0';

  // Print out the request
  printf( "name=%s\n", name );

  //Capture the host
  char *hostFind;
  char *hostMe;
  if(hostFind = strstr(name, "Host:")){
    hostFind+=6;
    int hostlen = 0;
    while(hostFind[hostlen] != '\n'){
      hostlen++;
    }
    hostMe = (char *)calloc(sizeof(char), hostlen);
    int copyHost = 0;
    while(copyHost < hostlen){
      hostMe[copyHost] = hostFind[copyHost];
      copyHost++;
    }
  }
  else{
    hostMe = (char *)calloc(sizeof(char), 4);
    hostMe = strdup("None");
  }

  // Change the directory so that we are in the directory that
  // is the default web directory
  chdir("http-root-dir");
  chdir("htdocs");

  FILE * logHandle = fopen("logs.html", "r");
  if(logHandle == NULL){
    logHandle = fopen("logs.html", "w");
    fprintf(logHandle, "<HTML><HEAD><TITLE>Server Log</TITLE></HEAD>\n");
    fclose(logHandle);
  }
  else{
    fclose(logHandle);
  }
  logHandle = fopen("logs.html", "a");

  int count = 0;
  while(name[count] != '/'){
    count++;
  }
  count++;
  int copy = 0;
  char *file = (char *)calloc(sizeof(char), 1024);
  while(name[count] != '\015' && name[count] != '\012' && name[count] != ' ' && name[count] != '\t'){
    file[copy] = name[count];
    copy++;
    count++;
  }

  //Section to handle Post request
  bool postFlag = false;
  int numPostChars = 0;
  char *queryStringPost;
  if(strstr(name, "POST") != NULL){
    postFlag = true;
    char * contentLen = strstr(name, "Content-Length: ");
    contentLen+=16;
    int numNumbers = 0;
    while(isdigit(contentLen[numNumbers])){
      numNumbers++;
    }
    char *contentNumber = (char *)calloc(sizeof(char), numNumbers);
    int copyNumbers = 0;
    while(copyNumbers < numNumbers){
      contentNumber[copyNumbers] = contentLen[copyNumbers];
      copyNumbers++;
    }
    numPostChars = atoi(contentNumber);

    int readPostIt = 0;
    queryStringPost = (char *)calloc(sizeof(char), numPostChars);
    while(readPostIt < numPostChars && (n = read(fd, &newChar, sizeof(newChar)) > 0)){
      queryStringPost[readPostIt] = newChar;
      readPostIt++;
    }
  }

  // Null terminate the file name
  file[copy] = '\0';

  char cgiCheck[7];
  bool cgiFlag = false;
  strncpy(cgiCheck, file, 7);
  
  //detect that a cgi script is being requested
  for(int checkIt = 0; checkIt < 7; checkIt++){
    tolower(cgiCheck[checkIt]);
  }

  //change our current directory to be the cgi-bin
  if(strstr(cgiCheck, "cgi-bin") != NULL){
    cgiFlag = true;
    chdir("../cgi-bin");
  }

  //Count how many directories we have in the path
  int numSlash = 0;
  for(int countSlash = 0; countSlash < copy; countSlash++){
    if(file[countSlash] == '/'){
      numSlash++;
    }
  }
  //Declare a flag to look for stats
  bool statsFlag = false;
  
  if(strcmp(file, "stats") == 0){
    statsFlag = true;
  }

  //open up stream for ease of writing
  __gnu_cxx::stdio_filebuf<char> fbout(fd, ios::out, 100);
  ostream sockout(&fbout);

  //Look for violation in security policy
  int numdirs = 0;
  int countThrough = 0;

  //count number of directories
  while(file[countThrough] != '\0'){
    if(file[countThrough] == '/'){
      numdirs++;
    }
    countThrough++;
  }

  //count number of ../
  int countDirUp = 0;
  countThrough = 0;
  char *instanceDirUp;
  while(file[countThrough] != '\0'){
    if(file[countThrough] == '.' && file[countThrough+1] == '.' && file[countThrough+2] == '/'){
      countDirUp++;
    }
    countThrough++;
  }

  if(countDirUp > numdirs - countDirUp){
    sockout << "HTTP/1.1 400 Bad Request\n";
    sockout << "Server: CS 354 lab5\n";
    sockout << "Content-type: text/html\n\n";
    sockout << "<HTML><HEAD><TITLE>400 Bad Request</TITLE>" << endl;
    sockout << "</HEAD><H1>400 Bad Request</H1></BODY></HTML>" << endl;
    return;
  }

  //Backup the filename for stat purposes
  char *fileBackup = strdup(file);

  //Write to log file
  fprintf(logHandle, "Host = %s<BR>\n", remoteHost);
  if(strcmp(fileBackup, "") == 0){
    fprintf(logHandle, "File Requested = index.html<BR><BR>\n", fileBackup);
  }
  else{
    fprintf(logHandle, "File Requested = %s<BR><BR>\n", fileBackup);
  }
  fclose(logHandle);

  //call the recursion to get to the directory in question
  if(strcmp(file, "logs") == 0){
    strcpy(file, "logs.html");
  }
  file = recurseSlash(file, numSlash);

  if(copy == 0){
    strcpy(file, "index.html");
  }

  bool loadableFlag = false;
  if(cgiFlag == true){
    int findQuestion = 0;
    while(file[findQuestion] != '\0' && file[findQuestion] != '?'){
      findQuestion++;
    }
    if(file[findQuestion - 1] == 'o' && file[findQuestion - 2] == 's' && file[findQuestion - 3] == '.'){
      loadableFlag = true;
    }
    else{
      putenv("SERVER_SOFTWARE = CS 354 Server");
      char *getInfo = strstr(name, "Referer:");
      if(getInfo != NULL){
        getInfo+=9;
        int getURLlen = 0;
        while(getInfo[getURLlen] != '\n'){
          getURLlen++;
        }
        char *refererURL = (char *)calloc(sizeof(char), getURLlen + 8);
        int getStuff = 0;
        while(getStuff < getURLlen-1){
          refererURL[getStuff] = getInfo[getStuff];
          getStuff++;
        }
        strcat(refererURL, "cgi-bin/");
	char * rurl = strdup ("SERVER_NAME = ");
	strcat(rurl, refererURL);
        putenv(rurl);
      }
      putenv("GATEWAY_INTERFACE = CGI/1.1");
      getInfo = strstr(name, "HTTP");
      if(getInfo != NULL){
        int numCharsEnd = 0;
        while(*getInfo != '\n'){
          getInfo++;
          numCharsEnd++;
        }
        while(*getInfo != ' '){
          getInfo--;
        }
        getInfo++;
        char * getHTTPVer = (char *)calloc(sizeof(char), numCharsEnd);
        int doIt = 0;
        while(getInfo[doIt] != '\n'){
          getHTTPVer[doIt] = getInfo[doIt];
          doIt++;
        }
	char * protenv = strdup ("SERVER_PROTOCOL = ");
	strcat(protenv, getHTTPVer);
        putenv(protenv);
      }
      char *portNumStr = (char *)calloc(sizeof(char), 5);
      sprintf(portNumStr, "%d", portNum);
      char * sport = strdup ("SERVER_PORT = ");
	strcat(sport, portNumStr);
      putenv(sport);
      char *acceptStringGet;
      acceptStringGet = strstr(name, "Accept");
      acceptStringGet += 8;
      int acceptLen = 0;
      while(acceptStringGet[acceptLen] != '\n'){
        acceptLen++;
      }
      char *accStr = (char *)calloc(sizeof(char), acceptLen);
      int copyIt = 0;
      while(copyIt < acceptLen){
        accStr[copyIt] = acceptStringGet[copyIt];
        copyIt++;
      }
      char * httpaccept = (char *)calloc(sizeof(char), 15); 
      httpaccept ="HTTP_ACCEPT = ";
      strcat(httpaccept, accStr);
      putenv(httpaccept);
      free(httpaccept);
      char *cgiPathShiz = (char *)calloc(sizeof(char), 2048);
      getcwd(cgiPathShiz, 2048);
      char * ptrans = (char *)calloc(sizeof(char), 20);
      ptrans = "PATH_TRANSLATED = ";
      strcat(ptrans, cgiPathShiz);
      putenv(ptrans);
      free(ptrans);
      
      char * scriptnm = (char *)calloc(sizeof(char), 15);
      scriptnm ="SCRIPT_NAME = ";
      strcat(scriptnm, file);
      putenv(scriptnm);
      free(scriptnm);
      if(remoteAddy != NULL){
	char * rmhost = (char *)calloc(sizeof(char), 15); 
	rmhost = "REMOTE_HOST = ";
	strcat(rmhost, remoteAddy);
        putenv(rmhost);
	free(rmhost);
      }
      char * rmaddy =(char *)calloc(sizeof(char), 15);
      rmaddy = "REMOTE_ADDR = ";
      strcat(rmaddy, remoteHost);
      putenv(rmaddy);
      free(rmaddy);
    }
  }
  //Open up pipe to feed it the arguments for CGI if the cgi flag is true
  char fileNoCGI[256];
  if(cgiFlag == true){
    char **argMe;
    char *queryString = (char *)calloc(sizeof(char), 2048);
    bool argsFlag = false;

    //If its a post request we get query string elsewhere
    if(postFlag == true){
      queryString = strdup(queryStringPost);
      argsFlag = true;
    }

    sockout << "HTTP/1.1 200 Document follows" << endl;
    sockout << "Server: CS 354 lab5" << endl;
    //copy over the filename to execute
    int copyMe = 0;
    while(file[copyMe] != '\0' && file[copyMe] != '?'){
      fileNoCGI[copyMe] = file[copyMe];
      copyMe++;
    }
    fileNoCGI[copyMe] = '\0';
    if(file[copyMe] == '?'){
      argsFlag = true;
      int queryCount = 0;
      copyMe++;
      while(file[copyMe] != '\0'){
        queryString[queryCount] = file[copyMe];
        queryCount++;
        copyMe++;
      }
      queryString[queryCount] = '\0';
    }
    if(loadableFlag == true){
      httprunfunc httpfunc;
      module *loaded = checkLoaded(fileNoCGI);

      if(loaded != NULL)
      {
        fprintf(stderr, "%s already loaded into memory", fileNoCGI);
      }
      else
      {
        loaded = (module *)malloc(sizeof(module));

        loaded->filename = strdup(fileNoCGI);

        char *dlopenpath = (char *)malloc(sizeof(char)*sizeof(fileNoCGI + 2));
        dlopenpath[0] = '.';
        dlopenpath[1] = '/';
        dlopenpath[2] = '\0';

        strcat(dlopenpath, fileNoCGI);

        
        putenv("LD_LIBRARY_PATH = .");
        loaded->library = dlopen(dlopenpath, RTLD_LAZY);


        if(loaded->library == NULL)
        {
          fprintf(stderr, "%s not found", fileNoCGI);
          perror("dlopen");
          exit(1);
        }

        modulesLoaded[numModulesLoaded].filename, strdup(loaded->filename);
        modulesLoaded[numModulesLoaded].library = loaded->library;
        // memcpy(modulesLoaded[numModulesLoaded], loaded, sizeof(module *));
        numModulesLoaded++;
      }

      httpfunc = (httprunfunc)dlsym(loaded->library, "httprun");
      if(httpfunc == NULL)
      {
        perror("dlsym: httprun not found");
        exit(1);
      }

      httpfunc(fd, queryString);
      close(fd);
      return;
    }
    else{
      if(argsFlag == true){
        //set up the environmental variables
	char * qstr = strdup ("QUERY_STRING = ");
	strcat(qstr, queryString);
        putenv(qstr);
        putenv("REQUEST_METHOD = GET");

        int countplus = 0;
        int tracer = 0;
        while(queryString[tracer] != '\0'){
          if(queryString[tracer] == '+'){
            countplus++;
          }
          tracer++;
        }
        countplus++;
        argMe = (char **)calloc(sizeof(char *), countplus);
        tracer = 0;
        int argNumber = 1;
        while(queryString[tracer] != '\0'){
          int inner = tracer;
          while(queryString[inner] != '+' && queryString[inner] != '\0'){
            inner++;
          }
          int copyAmount = inner - tracer;
          int copyIt = 0;
          char *newArg = (char *)calloc(sizeof(char), ++copyAmount);
          while(copyIt < copyAmount){
            newArg[copyIt] = queryString[tracer];
            tracer++;
            copyIt++;
          }
          argMe[argNumber] = strdup(newArg);
          argNumber++;
        }
      }
      else{
        putenv("REQUEST_METHOD = GET");
        argMe = (char **)calloc(sizeof(char *), 1);
      }
      argMe[0] = strdup(fileNoCGI);

      int backOut = dup(1);
      dup2(fd, 1);

      char *localPath = (char *)calloc(sizeof(char), strlen(fileNoCGI) + 2);
      strcpy(localPath, "./");
      strcat(localPath, fileNoCGI);
      int forking = fork();
      if(forking==0){
        execvp(localPath, (char* const*)argMe);
        exit(0);
      }
      wait(&forking);
      dup2(backOut, 1);
      //clearenv();
      putenv("environ = NULL");
    }
    return;
  }

  bool dirSweetFlag = false;
  if(strcmp(file, "") == 0 || opendir(file) != NULL){
    if(strcmp(file, "") != 0){
      chdir(file);
    }
    dirSweetFlag = true;
    //If we got in here then this is a directory search
    char *currDir = (char *)calloc(sizeof(char), 1024);
    getcwd(currDir, 1024);
    if(opendir(file) != NULL){
      strcat(currDir, "/");
      strcat(currDir, file);
    }
    DIR *thisDir = opendir(currDir);
    if(thisDir != NULL){
      //Break out what the directory is that is being displayed
      int slashStart = strlen(currDir) - 1;
      if(currDir[slashStart] == '/'){
        slashStart--;
      }
      while(currDir[slashStart] != '/'){
        slashStart--;
      }
      slashStart++;
      char *dirName = (char *)calloc(sizeof(char), strlen(currDir) - slashStart + 1);
      int fillDir = 0;
      while(currDir[slashStart] != '/' && currDir[slashStart] != '\0'){
        dirName[fillDir] = currDir[slashStart];
        fillDir++;
        slashStart++;
      }

      struct dirent *theDir;
      write(fd, "HTTP/1.1 200 Document follows\n", 30);
      write(fd, "Server: CS 354 lab3\n", 20);
      write(fd, "Content-type: text/html\n\n", 25);
      write(fd, "<HTML>\n<HEAD>\n<TITLE>Directory ", 31);
      write(fd, dirName, strlen(dirName));
      write(fd, "</TITLE>\n", 9);
      write(fd, "<Script Language=\"javascript\">\n", 31);
      int numFiles = 0;
      while(theDir = readdir(thisDir)){
        if(strcmp(theDir->d_name, ".") != 0){
          numFiles++;
        }
      }
      closedir(thisDir);
      thisDir = opendir(currDir);

      //generate code for javascript array for filenames
      write(fd, "var dirStuff=new Array(", 23);
      char *numberString = (char *)calloc(sizeof(char), 100);
      sprintf(numberString, "%d", numFiles);
      write(fd, numberString, strlen(numberString));
      write(fd, ");\n", 3);

      //generate code for javascript duplicate for filenames
      write(fd, "var dirStuff2=new Array(", 24);
      sprintf(numberString, "%d", numFiles);
      write(fd, numberString, strlen(numberString));
      write(fd, ");\n", 3);

      //generate code for javascript duplicate for filenames
      write(fd, "var fullPath=new Array(", 23);
      sprintf(numberString, "%d", numFiles);
      write(fd, numberString, strlen(numberString));
      write(fd, ");\n", 3);

      //generate code for javascript duplicate for filenames
      write(fd, "var fullPath2=new Array(", 24);
      sprintf(numberString, "%d", numFiles);
      write(fd, numberString, strlen(numberString));
      write(fd, ");\n", 3);

      //generate code for javascript array for filesize
      write(fd, "var sizeStuff=new Array(", 24);
      sprintf(numberString, "%d", numFiles);
      write(fd, numberString, strlen(numberString));
      write(fd, ");\n", 3);

      //generate code for javascript duplicate array for filesize
      write(fd, "var sizeStuff2=new Array(", 25);
      sprintf(numberString, "%d", numFiles);
      write(fd, numberString, strlen(numberString));
      write(fd, ");\n", 3);

      //generate code for javascript array for modDate
      write(fd, "var modDate=new Array(", 22);
      sprintf(numberString, "%d", numFiles);
      write(fd, numberString, strlen(numberString));
      write(fd, ");\n", 3);

      //generate code for javascript duplicate array for modDate
      write(fd, "var modDate2=new Array(", 23);
      sprintf(numberString, "%d", numFiles);
      write(fd, numberString, strlen(numberString));
      write(fd, ");\n", 3);

      //generate code for javascript array for modDate
      write(fd, "var date=new Array(", 19);
      sprintf(numberString, "%d", numFiles);
      write(fd, numberString, strlen(numberString));
      write(fd, ");\n", 3);

      //generate code for javascript duplicate array for modDate
      write(fd, "var date2=new Array(", 20);
      sprintf(numberString, "%d", numFiles);
      write(fd, numberString, strlen(numberString));
      write(fd, ");\n", 3);
      
      int arrFill = 0;
      while(theDir = readdir(thisDir)){
        if(strcmp(theDir->d_name, ".") != 0){
          write(fd, "dirStuff[", 9);
          sprintf(numberString, "%d", arrFill);
          write(fd, numberString, strlen(numberString));
          write(fd, "]=\"", 3);
          if(strcmp(theDir->d_name, "..") == 0 || strcmp(theDir->d_name, "../") == 0){
            if(theDir->d_name[strlen(theDir->d_name) - 1] == '/'){
              write(fd, "../",3);
            }
            else{
              write(fd, "/../",4);
            }
          }
          else{
            write(fd, theDir->d_name, strlen(theDir->d_name));
          }
          write(fd, "\";\n", 3);

          write(fd, "fullPath[", 9);
          sprintf(numberString, "%d", arrFill);
          write(fd, numberString, strlen(numberString));
          write(fd, "]=\"", 3);
          if(strcmp(file, "") != 0 && strcmp(theDir->d_name, "..") != 0){
            write(fd, file, strlen(file));
            write(fd, "/", 1);
          }
          if(strcmp(theDir->d_name, "..") == 0 || strcmp(theDir->d_name, "../") == 0){
            if(strcmp(file, "") != 0){
              write(fd, dirName, strlen(dirName));
            }
            if(theDir->d_name[strlen(theDir->d_name) - 1] == '/'){
              write(fd, "../",3);
            }
            else{
              write(fd, "/../",4);
            }
          }
          else{
            write(fd, theDir->d_name, strlen(theDir->d_name));
          }
          write(fd, "\";\n", 3);
        
          write(fd, "dirStuff2[", 10);
          write(fd, numberString, strlen(numberString));
          write(fd, "]=dirStuff[", 11);
          sprintf(numberString, "%d", arrFill);
          write(fd, numberString, strlen(numberString));
          write(fd, "];\n", 3);

          //This writes the variables to track the link
          write(fd, "fullPath2[", 10);
          write(fd, numberString, strlen(numberString));
          write(fd, "]=fullPath[", 11);
          sprintf(numberString, "%d", arrFill);
          write(fd, numberString, strlen(numberString));
          write(fd, "];\n", 3);


          //get file size and write it
          struct stat *fileSizeMe = new struct stat;
          stat(theDir->d_name, fileSizeMe);
          char *tempIt = (char *)calloc(sizeof(char), 100);
          sprintf(tempIt, "%d", fileSizeMe->st_size);

          //write the variables to the socket (fileSize)
          write(fd, "sizeStuff[", 10);
          sprintf(numberString, "%d", arrFill);
          write(fd, numberString, strlen(numberString));
          write(fd, "]=", 2);
          write(fd, tempIt, strlen(tempIt));
          write(fd, ";\n", 2);
          write(fd, "sizeStuff2[", 11);
          write(fd, numberString, strlen(numberString));
          write(fd, "]=sizeStuff[", 12);
          write(fd, numberString, strlen(numberString));
          write(fd, "];\n", 3);

          //get time and write it
          time_t modDate = fileSizeMe->st_mtime;
          sprintf(tempIt, "%d", modDate);

          //write the variables to the socket (modTime)
          write(fd, "modDate[", 8);
          sprintf(numberString, "%d", arrFill);
          write(fd, numberString, strlen(numberString));
          write(fd, "]=", 2);
          write(fd, tempIt, strlen(tempIt));
          write(fd, ";\n", 2);
          write(fd, "modDate2[", 9);
          write(fd, numberString, strlen(numberString));
          write(fd, "]=modDate[", 10);
          write(fd, numberString, strlen(numberString));
          write(fd, "];\n", 3);
          write(fd, "date[", 5);
          write(fd, numberString, strlen(numberString));
          write(fd, "]=\"", 3);
          write(fd, ctime(&modDate), strlen(ctime(&modDate)) - 1);
          write(fd, "\";\n", 3);
          write(fd, "date2[", 6);
          write(fd, numberString, strlen(numberString));
          write(fd, "]=\"", 3);
          write(fd, ctime(&modDate), strlen(ctime(&modDate)) - 1);
          write(fd, "\";\n", 3);

          arrFill++;
        }
      }

      //write a function to initialize table
      write(fd, "function fill(){", 16);
      write(fd, "var track;", 10);
      write(fd, "var x = document.getElementById('tableMe').rows;", 48);
      write(fd, "var y;", 6);
      write(fd, "for(track = 0; track <= ", 24);
      write(fd, numberString, strlen(numberString));
      write(fd, "; track++){", 11);
      write(fd, "y=x[track+1].cells;", 19);
      write(fd, "y[0].innerHTML='<A HREF=' + fullPath[track] + '>' + dirStuff[track] + '</A>';", 77);
      write(fd, "y[1].innerHTML=sizeStuff[track];", 32);
      write(fd, "y[2].innerHTML=date[track];", 27);
      write(fd, "}}", 2);

      //write the body of the page
      write(fd, "function printMe(which){\n", 25);
      write(fd, "var a;", 6);
      write(fd, "var b;", 6);
      write(fd, "var x = document.getElementById('tableMe').rows;", 48);
      write(fd, "var y;", 6);
      
      
      //This takes care of sorting for filenames
      write(fd, "if(which==1){", 13);
      write(fd, "for(a=0;a<=", 11);
      write(fd, numberString, strlen(numberString));
      write(fd, ";a++){", 6);
      write(fd, "for(b=0;b<=", 11);
      write(fd, numberString, strlen(numberString));
      write(fd, ";b++){", 6);
      write(fd, "if(dirStuff[a]<dirStuff[b]){", 28);
      write(fd, "var swap=dirStuff[a];", 21);
      write(fd, "dirStuff[a]=dirStuff[b];", 24);
      write(fd, "dirStuff[b]=swap;", 17);
      write(fd, "swap=sizeStuff[a];", 18);
      write(fd, "sizeStuff[a]=sizeStuff[b];", 26);
      write(fd, "sizeStuff[b]=swap;", 18);
      write(fd, "swap=fullPath[a];", 17);
      write(fd, "fullPath[a]=fullPath[b];", 24);
      write(fd, "fullPath[b]=swap;", 17);
      write(fd, "swap=date[a];", 13);
      write(fd, "date[a]=date[b];", 16);
      write(fd, "date[b]=swap;", 13);
      write(fd, "swap=modDate[a];", 16);
      write(fd, "modDate[a]=modDate[b];", 22);
      write(fd, "modDate[b]=swap;}}}", 19);
      write(fd, "for(a=0;a<=", 11);
      write(fd, numberString, strlen(numberString));
      write(fd, ";a++){", 6);
      write(fd, "y=x[a+1].cells;", 15);
      write(fd, "y[0].innerHTML='<A HREF=' + fullPath[a] + '>' + dirStuff[a] + '</A>';", 69);
      write(fd, "y[1].innerHTML=sizeStuff[a];", 28);
      write(fd, "y[2].innerHTML=date[a];}", 24);
      write(fd, "for(a=0;a<=", 11);
      write(fd, numberString, strlen(numberString));
      write(fd, ";a++){", 6);
      write(fd, "dirStuff[a]=dirStuff2[a];", 25);
      write(fd, "sizeStuff[a]=sizeStuff2[a];", 27);
      write(fd, "fullPath[a]=fullPath2[a];", 25);
      write(fd, "date[a]=date2[a];", 17);
      write(fd, "modDate[a]=modDate2[a];}}", 25);

      
      //This takes care of sorting file size
      write(fd, "if(which==2){", 13);
      write(fd, "for(a=0;a<=", 11);
      write(fd, numberString, strlen(numberString));
      write(fd, ";a++){", 6);
      write(fd, "for(b=0;b<=", 11);
      write(fd, numberString, strlen(numberString));
      write(fd, ";b++){", 6);
      write(fd, "if(sizeStuff[a]<sizeStuff[b]){", 30);
      write(fd, "var swap=dirStuff[a];", 21);
      write(fd, "dirStuff[a]=dirStuff[b];", 24);
      write(fd, "dirStuff[b]=swap;", 17);
      write(fd, "swap=fullPath[a];", 17);
      write(fd, "fullPath[a]=fullPath[b];", 24);
      write(fd, "fullPath[b]=swap;", 17);
      write(fd, "swap=sizeStuff[a];", 18);
      write(fd, "sizeStuff[a]=sizeStuff[b];", 26);
      write(fd, "sizeStuff[b]=swap;", 18);
      write(fd, "swap=date[a];", 13);
      write(fd, "date[a]=date[b];", 16);
      write(fd, "date[b]=swap;", 13);
      write(fd, "swap=modDate[a];", 16);
      write(fd, "modDate[a]=modDate[b];", 22);
      write(fd, "modDate[b]=swap;}}}", 19);
      write(fd, "for(a=0;a<=", 11);
      write(fd, numberString, strlen(numberString));
      write(fd, ";a++){", 6);
      write(fd, "y=x[a+1].cells;", 15);
      write(fd, "y[0].innerHTML='<A HREF=' + fullPath[a] + '>' + dirStuff[a] + '</A>';", 69);
      write(fd, "y[1].innerHTML=sizeStuff[a];", 28);
      write(fd, "y[2].innerHTML=date[a];}", 24);
      write(fd, "for(a=0;a<=", 11);
      write(fd, numberString, strlen(numberString));
      write(fd, ";a++){", 6);
      write(fd, "dirStuff[a]=dirStuff2[a];", 25);
      write(fd, "sizeStuff[a]=sizeStuff2[a];", 27);
      write(fd, "fullPath[a]=fullPath2[a];", 25);
      write(fd, "date[a]=date2[a];", 17);
      write(fd, "modDate[a]=modDate2[a];}}", 25);

      //This takes care of sorting mod size
      write(fd, "if(which==3){", 13);
      write(fd, "for(a=0;a<=", 11);
      write(fd, numberString, strlen(numberString));
      write(fd, ";a++){", 6);
      write(fd, "for(b=0;b<=", 11);
      write(fd, numberString, strlen(numberString));
      write(fd, ";b++){", 6);
      write(fd, "if(modDate[a]<modDate[b]){", 26);
      write(fd, "var swap=dirStuff[a];", 21);
      write(fd, "dirStuff[a]=dirStuff[b];", 24);
      write(fd, "dirStuff[b]=swap;", 17);
      write(fd, "swap=sizeStuff[a];", 18);
      write(fd, "sizeStuff[a]=sizeStuff[b];", 26);
      write(fd, "sizeStuff[b]=swap;", 18);
      write(fd, "swap=fullPath[a];", 17);
      write(fd, "fullPath[a]=fullPath[b];", 24);
      write(fd, "fullPath[b]=swap;", 17);
      write(fd, "swap=date[a];", 13);
      write(fd, "date[a]=date[b];", 16);
      write(fd, "date[b]=swap;", 13);
      write(fd, "swap=modDate[a];", 16);
      write(fd, "modDate[a]=modDate[b];", 22);
      write(fd, "modDate[b]=swap;}}}", 19);
      write(fd, "for(a=0;a<=", 11);
      write(fd, numberString, strlen(numberString));
      write(fd, ";a++){", 6);
      write(fd, "y=x[a+1].cells;", 15);
      write(fd, "y[0].innerHTML='<A HREF=' + fullPath[a] + '>' + dirStuff[a] + '</A>';", 69);
      write(fd, "y[1].innerHTML=sizeStuff[a];", 28);
      write(fd, "y[2].innerHTML=date[a];}", 24);
      write(fd, "for(a=0;a<=", 11);
      write(fd, numberString, strlen(numberString));
      write(fd, ";a++){", 6);
      write(fd, "dirStuff[a]=dirStuff2[a];", 25);
      write(fd, "sizeStuff[a]=sizeStuff2[a];", 27);
      write(fd, "fullPath[a]=fullPath2[a];", 25);
      write(fd, "date[a]=date2[a];", 17);
      write(fd, "modDate[a]=modDate2[a];}}", 25);


      write(fd, "}</Script></HEAD><BODY onload=\"javascript:fill()\">", 50);
      write(fd, "<H1>", 4);
      write(fd, dirName, strlen(dirName));
      write(fd, "</H1>", 5);
      write(fd, "<table id=\"tableMe\" border=\"1\">", 32);
      write(fd, "<tr>", 4);
      write(fd, "<td><a href=\"javascript:printMe(1);\">File</a><br></td>", 54);
      write(fd, "<td><a href=\"javascript:printMe(2);\">File Size</a><br></td>", 59);
      write(fd, "<td><a href=\"javascript:printMe(3);\">Modification Time</a><br></td>", 67);
      write(fd, "</tr>", 5);
      
      for(int makeCells = 0; makeCells <= arrFill; makeCells++){
        write(fd, "<tr><td></td><td></td><td></td>", 31);
      }
      write(fd, "</table></BODY></HTML>", 22);
    }
  }
  FILE * deez = fopen(file, "r");
  void *buffer = malloc(1000);

  //Throw a special flag to look to see if the cgi file exists
  if(cgiFlag == true){
    deez = fopen(fileNoCGI, "r");
  }
 
  //Write the correct header file to the server
  if(dirSweetFlag == false){
    if(deez == NULL && statsFlag == false){
      write(fd, "HTTP/1.1 404 File Not Found\n", 28);
      write(fd, "Server: CS 354 lab5\n", 20);
      write(fd, "Content-type: text/html\n\n", 25);
      write(fd, "<H1>404 File not Found</H1>\n", 24);
      write(fd, "\n<HTML><HEAD><TITLE>404 FILE NOT FOUND</TITLE></HEAD></HTML>", 59);
    }
    else if(statsFlag == true){
      //This section writes the stats page when requested
      sockout << "HTTP/1.1 200 Document follows\n";
      sockout << "Server: CS 354 lab5\n";
      sockout << "Content-type: text/html\n\n";
      time_t clockIt = time(NULL);
      int clockDiff = clockIt - timeIt;
      int numDays = clockDiff / 86400;
      int numHours = clockDiff % 86400 / 3600;
      int numMin = clockDiff % 86400 % 3600 / 60;
      int numSec = clockDiff % 86400 % 3600 % 60;
      sockout << "<HTML><HEAD><TITLE>Server Statistics</TITLE></HEAD><BODY><H1>Server Statistics</H1>Moyukh Sen<BR>Server up for " << endl;
      sockout << numDays << " Days " << numHours << " Hours " << numMin << " Minutes and " << numSec << " Seconds<BR>" << endl;
      if(timesCalled == 1){
        sockout << "There has been " << timesCalled << " request since startup<BR>" << endl;
      }
      else{
        sockout << "There have been " << timesCalled << " requests since startup<BR>" << endl;
      }
      if(shortTime != -1){
        sockout << "The Minimum service request time was " << shortTime << " microseconds for " << shortURL << "<BR>" << endl;
        sockout << "The Maximum service request time was " << longTime << " microseconds for " << longURL << "<BR>" << endl;
      }
    }
    else if(cgiFlag != true){
      sockout << "HTTP/1.1 200 Document follows" << endl;
      sockout << "Server: CS 354 lab5" << endl;
      int fileLength = strlen(file);
      if(tolower(file[fileLength -1]) == 'f' && tolower(file[fileLength -2]) == 'i' && tolower(file[fileLength-3]) == 'g' && file[fileLength-4] == '.'){
        sockout << "Content-type: image/gif\n" << endl;;
      }
      else{
        if(cgiFlag == false){
          sockout << "Content-type: text/html\n" << endl;
        }
      }
    } 
  }

  if(deez != NULL && cgiFlag == false && statsFlag == false){
    char *charMe;

    int outNum;
    while(outNum = fread(buffer, 1, 4, deez)){
      write(fd, buffer, outNum);
    }
  }
  //restore current working directory;
  chdir(curDir);
  
  time (&end);

//  timer_gettime(*timeReq, &value);
  double totalTimeTaken = difftime (end,start);

  if(longTime == -1 || totalTimeTaken > longTime){
    longURL = strdup(fileBackup);
    longTime = totalTimeTaken;
  }
  if(shortTime == -1 || totalTimeTaken < shortTime){
    shortURL = strdup(fileBackup);
    shortTime = totalTimeTaken;
  }

  close(fd);

}

char * recurseSlash(char *path, int numSlash){
  if(numSlash == 0){
    return path;
  }
  int countSlash = 0;
  int firstSlash = -1;
  for(int slashIt = 0; path[slashIt] != '\0'; slashIt++){
    if(path[slashIt] == '/'){
      countSlash++;
      if(firstSlash == -1){
        firstSlash = slashIt;
      }
    }
  }
  
  //allocate appropriate space for directory change
  char *newDir = (char *)calloc(sizeof(char), firstSlash);

  //pull out the directory 
  //then change to this directory
  for(int changeSlash = 0; changeSlash < firstSlash; changeSlash++){
    newDir[changeSlash] = path[changeSlash];
  }
  chdir(newDir);
  for(int advanceSlash = 0; advanceSlash <= firstSlash; advanceSlash++){
    *path++;
  }

  return recurseSlash(path, numSlash - 1);
}

extern void zombie(int sig)
{
  int pid = wait3(&pid, WNOHANG, NULL);
  printf("%d exited.", pid);
}

extern void ignore(int sig)
{

}

void threadServer(int *masterSocket)
{
  while(1)
  {
    int masterSock = *masterSocket;
    struct sockaddr_in clientIPAddress;
    int alen = sizeof( clientIPAddress );
    pthread_mutex_lock(&mutexMe);
    int slaveSocket = accept( masterSock, (struct sockaddr *)&clientIPAddress, (socklen_t*)&alen);
    struct hostent *addrGet = gethostbyaddr((char *)&clientIPAddress.sin_addr, alen, AF_INET);
    if(addrGet != NULL){
      remoteAddy = addrGet->h_name;
    }
    remoteHost = inet_ntoa(clientIPAddress.sin_addr);
    pthread_mutex_unlock(&mutexMe);

    if ( slaveSocket < 0 ) {
      perror( "accept" );
      exit( -1 );
    }     
    processTimeRequest(slaveSocket);
  }
}

module *checkLoaded(char *file)
{
  int i = 0;

  while(modulesLoaded[i].filename != NULL)
    {
      if(strcmp(modulesLoaded[i].filename, file) == 0)
	{
	  return (modulesLoaded + i);
	}
      i++;
    }

  return NULL;
}
