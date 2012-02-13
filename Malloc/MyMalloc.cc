//
// CS354: MyMalloc Project
//
// The current implementation gets memory from the OS
// every time memory is requested and never frees memory.
//
// You will implement the allocator as indicated in the handout.
// 
// Also you will need to add the necessary locking mechanisms to
// support multi-threaded programs.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

enum {
  ObjFree = 0,
  ObjAllocated = 1
};

// Header of an object. Used both when the object is allocated and freed
class ObjectHeader {
 public:
  int _flags;		      // flags == ObjFree or flags = ObjAllocated
  size_t _objectSize;         // Size of the object. Used both when allocated
			      // and freed.
  void * previous;
  void * next;
};

class ObjectFooter {
 public:
  int _flags;		      // flags == ObjFree or flags = ObjAllocated
  size_t _objectSize;         // Size of the object. Used both when allocated
			      // and freed.
  void * previous;
  void * next;
};

class Allocator {
  // State of the allocator

  // Size of the heap
  size_t _heapSize;

  // True if heap has been initialized
  int _initialized;

  // Verbose mode
  int _verbose;

  // # malloc calls
  int _mallocCalls;

  // # free calls
  int _freeCalls;

  // # realloc calls
  int _reallocCalls;
  
  // # calloc calls
  int _callocCalls;
  
  void * shitBrick[65];

  pthread_mutex_t mutex;

  struct Node{
	  void * _next;
	  void * _previous;
  };
  
public:
  // This is the only instance of the allocator.
  static Allocator TheAllocator;

  //Initializes the heap
  void initialize();

  // Allocates an object 
  void * allocateObject( size_t size );

  // Frees an object
  void freeObject( void * ptr );

  // Returns the size of an object
  size_t objectSize( void * ptr );

  // At exit handler
  void atExitHandler();

  //Prints the heap size and other information about the allocator
  void print();

  // Gets memory from the OS
  void * getMemoryFromOS( size_t size );

  void increaseMallocCalls() { _mallocCalls++; }

  void increaseReallocCalls() { _reallocCalls++; }

  void increaseCallocCalls() { _callocCalls++; }

  void increaseFreeCalls() { _freeCalls++; }

};

Allocator Allocator::TheAllocator;

extern "C" void
atExitHandlerInC()
{
  Allocator::TheAllocator.atExitHandler();
}

void
Allocator::initialize()
{
  // Environment var VERBOSE prints stats at end and turns on debugging
  // Default is on
  _verbose = 1;
  const char * envverbose = getenv( "MALLOCVERBOSE" );
  if ( envverbose && !strcmp( envverbose, "NO") ) {
    _verbose = 0;
  }

  // In verbose mode register also printing statistics at exit
  atexit( atExitHandlerInC );

  _initialized = 1;
  
  for (int i = 0; i <65; i++)
  {
	  shitBrick[i] = NULL;
  }


  pthread_mutex_init(&mutex, NULL);
  pthread_attr_t attr;

  pthread_attr_init( &attr );
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
}

void *
Allocator::allocateObject( size_t size )
{
  //pthread_mutex_lock(&mutex);
  // Simple implementation

  //flag for list 64
  int fag64 = 0;

  //Make sure that allocator is initialized
  if ( !_initialized ) {
    initialize();
  }

  //SZ: size needs to be at least 8 i thnk? if less than 8 change to 8
  if( size < 8)
    size = 8;

  printf("\nsize: %d\n", size);
  // Add the ObjectHeader to the size and round the total size up to a
  // multiple of 8 bytes for alignment.
   size_t totalSize = (size + sizeof(ObjectHeader) + sizeof(ObjectFooter) + 7) & ~7;
  //    size_t totalSize = (size + sizeof(ObjectHeader) + sizeof(ObjectFooter)) + (8 - (size + sizeof(ObjectHeader) + sizeof(ObjectFooter)) % 8);

  // You should get memory from the OS only if the memory in the free list could not
  // satisfy the request.
  
     printf("totalSize: %d\n", totalSize);
  int list = (totalSize >= 512? 64:((totalSize)>>3)-1);
  /*   int list;
    if (totalSize >= 512)
  		list = 64;
	else
		list = totalSize/8;
  */
  printf("original list: %d\n", list);

  if (shitBrick[list] != NULL && (list != 64))
  {

	  ObjectHeader * oh = (ObjectHeader *) shitBrick[list];
	  oh->_flags = ObjAllocated;
	  ObjectFooter * of = (ObjectFooter *) ((char*)shitBrick[list] + oh->_objectSize - sizeof(ObjectFooter));
	  of->_flags = ObjAllocated;
	  
	  
	  void * returnSpace = (char *)shitBrick[list] + sizeof(ObjectHeader);

	  if(oh->next != NULL)
	  {
	    shitBrick[list] = oh->next;
	    ((ObjectHeader*)shitBrick[list])->previous = NULL;
	  }
	  else
	  {
	    shitBrick[list] = NULL;
	  }
	  return returnSpace;
  }
  else
  {
	  /*
	  for (int i = list; shitBrick[i] == NULL && i < 65; i++)
	  {
		  list = i+1;
	  }*/


	  for (int i = 64; i>list; i--)
	  {
	    if (shitBrick[i] != NULL)
	      {
		list = i;
	      }
	  }
	  if(list > 64)
		  list = 64;


	printf("modified list: %d\n", list);			  
	if(shitBrick[list] == NULL)
	  printf("list %d is NULL\n", list);

	//if list 64 and not empty, check to see if it contains big enough chunk
	if(shitBrick[list] != NULL && list == 64)
	{

	  ObjectHeader * ohTemp = (ObjectHeader *) shitBrick[list];
	  if(ohTemp->_objectSize <= totalSize)
	  {
	    fag64 = 1;
	  }
	  printf("loopObjectsize: %d\n", ohTemp->_objectSize);
	  
	  if(fag64 == 1)
	  {
	  while(ohTemp->next != NULL)
	  {
	    ohTemp = (ObjectHeader *)ohTemp->next;
	    printf("loopObjectsize2: %d\n", ohTemp->_objectSize);
	    if(ohTemp->_objectSize <= totalSize)
	    {
	      fag64 = 1;
	    }
	    else
	    {
	      fag64 = 0;
	      break;
	    }
	  }
	  }

	  /*
	  Node * iterator = (Node *)((char *)shitBrick[list] + sizeof(ObjectHeader));
	  while (iterator->_next != NULL)
	  {
	    iterator = (Node *)iterator->_next;
	    ohTemp = (ObjectHeader *) ((char *)iterator - sizeof(ObjectHeader));
	    printf("loopObjectsize2: %d\n", ohTemp->_objectSize);
	    if(ohTemp->_objectSize <= totalSize)
	    {
	      fag64 = 1;
	    }
	    else
	    {
	      fag64 = 0;
	      //break;
	    }
	      	      
	    
	  }
	  */
	  
	}

	printf("fag64: %d\n", fag64);

	  if (shitBrick[list] != NULL && fag64 == 0 )
	  {
		  //SPLIT UP AND MOVE SHIT

		  ObjectHeader * oh = (ObjectHeader *) shitBrick[list];
		  oh->_flags = ObjAllocated;
		  printf("objectsize: %d\n", oh->_objectSize);

		  //If chunk is exact size, remove chunk from list
		  if(oh->_objectSize == totalSize)
		  {
		    ObjectFooter * of = (ObjectFooter *) ((char*)shitBrick[list] + oh->_objectSize - sizeof(ObjectFooter));
		    of->_flags = ObjAllocated;
		  
		    
		    void * returnSpace = (char *)shitBrick[list] + sizeof(ObjectHeader);
		    


		    if( ((ObjectHeader *)shitBrick[list])->next != NULL)
		    {
		      shitBrick[list] = ((ObjectHeader*)shitBrick[list])->next;
		      ((ObjectHeader*)shitBrick[list])->previous = NULL;
		    }
		    else
		    {
		      shitBrick[list] = NULL;
		    }
		    
		    /*
		    //If the chunk has a next, set the next chunk to the first in the list
		    if(((Node*)((char*)shitBrick[list] + sizeof(ObjectHeader)))->_next != NULL)
		    {
		      shitBrick[list] = (char *)((Node *) ((char *)shitBrick[list] + sizeof(ObjectHeader)))->_next - sizeof(ObjectHeader);
		      ((Node*)((char*)shitBrick[list] + sizeof(ObjectHeader)))->_previous = NULL;
		    }
		    //if there are no more chunks in the list, set it equal to NULL
		    else
		    {
		      shitBrick[list] = NULL;
		    }
		    */

		    printf("ret size: %d\n", (int) oh->_objectSize);
		
		    return returnSpace;
		  }

		  //SPECIAL CASE FOR LIST 64 TO FIND BLOCK BIG ENOUGH
		  else if(list == 64 && (oh->_objectSize < totalSize))
		  {
		    while(oh->_objectSize < totalSize)
		    {
		      oh = (ObjectHeader *)oh->next;
		    }

		    //remove block from list
		    ((ObjectHeader *)oh->previous)->next = oh->next;
		    if(oh->next != NULL)
		    {
		      ((ObjectHeader *)oh->next)->previous = oh->previous;
		    }
		    
		    printf("originalSize: %d\n", oh->_objectSize);
		    int originalSize = oh->_objectSize;
		    oh->_objectSize = totalSize;
		    oh->_flags = ObjAllocated;

		    ObjectFooter * of = (ObjectFooter *)((char*)oh + totalSize - sizeof(ObjectFooter));
		    of->_objectSize = totalSize;
		    of->_flags = ObjAllocated;
		    

		    printf("returnSize: %d\n", oh->_objectSize);
		    void * returnSpace = (char *)oh + sizeof(ObjectHeader);

		    //movespace
		    void * moveSpace = (char*)of + sizeof(ObjectFooter);
		    ObjectHeader *ohomo = (ObjectHeader *)(moveSpace);
		    ohomo->_objectSize = originalSize - totalSize;
		    ohomo->_flags = ObjFree;

		    ObjectFooter * ofag = (ObjectFooter *)((char *)moveSpace + (originalSize - totalSize) - sizeof(ObjectFooter));
		    ofag->_objectSize = originalSize - totalSize;
		    ofag->_flags = ObjFree;
		    
		    int moveList = (ohomo->_objectSize >= 512? 64:((ohomo->_objectSize)>>3)-1);
		    
		    printf("newSize: %d\n", ohomo->_objectSize);

		    if (shitBrick[moveList] == NULL)
		    {
		      shitBrick[moveList] = moveSpace;
		      ObjectHeader * cunt = (ObjectHeader*)shitBrick[moveList];
		      cunt->next = NULL;
		      cunt->previous = NULL;
		      if(shitBrick[moveList] != NULL)
			printf("moveList: %d\n", moveList);
		    }
		    else
		    {
		      
		      //if 64, ascend
		      if(moveList == 64)
		      {
			ObjectHeader * moveObjHeader = (ObjectHeader*)moveSpace;
			
			ObjectHeader * cunt = (ObjectHeader*)shitBrick[moveList];
			
			if(moveObjHeader->_objectSize <= cunt->_objectSize)
			{
			  cunt->previous = moveSpace;
			  shitBrick[moveList] = moveSpace;
			  ((ObjectHeader *)shitBrick[moveList])->previous = NULL;
			  ((ObjectHeader *)shitBrick[moveList])->next = cunt;
			}
			else
			{
			  while(moveObjHeader->_objectSize > cunt->_objectSize && cunt->next != NULL)
			  {
			    cunt = (ObjectHeader *)cunt->next;
			  }
			  
			  if(cunt->next == NULL && moveObjHeader->_objectSize > cunt->_objectSize) //last in list
			  {
			    cunt->next = moveObjHeader;
			    moveObjHeader->next = NULL;
			    moveObjHeader->previous = cunt;
			  }
			  else                             //stuck in between somewhere
			  {
			    ((ObjectHeader*)cunt->previous)->next = moveObjHeader;
			    moveObjHeader->previous = cunt->previous;
			    cunt->previous = moveObjHeader;
			    moveObjHeader->next = cunt;
			  }
			}
		      }
		      else
		      {
			ObjectHeader * cunt = (ObjectHeader *)shitBrick[moveList];
			cunt->previous = moveSpace;
			shitBrick[moveList] = moveSpace;
			((ObjectHeader *)shitBrick[moveList])->previous = NULL;
			((ObjectHeader *)shitBrick[moveList])->next = cunt;
		      }
			   
		    }
		    
		    printf("TESTING\n");
		    return returnSpace;
		  }

		  //else if it needs to be split
		  //redundant of the above but doesn't need to find big enough block... just uses first block in list
		  else
		  {
		    printf("originalSize: %d\n", oh->_objectSize);
         		  int originalSize = oh->_objectSize;
		          oh->_objectSize = totalSize;
			  oh->_flags = ObjAllocated;
			  //ObjectFooter * of = (ObjectFooter *)((char *)shitBrick[list] + size + sizeof(ObjectHeader));
			  ObjectFooter * of = (ObjectFooter *)((char *)shitBrick[list] + totalSize - sizeof(ObjectFooter));
			  of->_objectSize = totalSize;
			  of->_flags = ObjAllocated;

			  printf("returnSize: %d\n", oh->_objectSize);
			  void * returnSpace = (char *)shitBrick[list] + sizeof(ObjectHeader);

						  

			  shitBrick[list] = ( (ObjectHeader *)shitBrick[list])->next;
			  //shitBrick[list] = ((Node *)((char *)shitBrick[list] + sizeof(ObjectHeader)))->_next;

			  
			   void * moveSpace = (char*)of + sizeof(ObjectFooter);
			  ObjectHeader * ohomo = (ObjectHeader *)(moveSpace);
			  ohomo->_objectSize = originalSize-totalSize;
			  ohomo->_flags = ObjFree;

			  
			  ObjectFooter * ofag = (ObjectFooter *)((char *)moveSpace + (originalSize-totalSize) - sizeof(ObjectFooter));
			  
			  ofag->_objectSize = originalSize-totalSize;
			  
			  ofag->_flags = ObjFree;

			  int moveList = (ohomo->_objectSize >= 512? 64:((ohomo->_objectSize)>>3)-1);


			  printf("newSize: %d\n", ohomo->_objectSize);
			  if (shitBrick[moveList] == NULL)
			  {
				  shitBrick[moveList] = moveSpace;
				  ObjectHeader * cunt = (ObjectHeader*)shitBrick[moveList];
				  cunt->next = NULL;
				  cunt->previous = NULL;
				  if(shitBrick[moveList] != NULL)
					  printf("moveList: %d\n", moveList);

				  /*
				  Node * cunt = (Node *) ((char *)shitBrick[moveList] + sizeof(ObjectHeader));
				  cunt->_next = NULL;
				  cunt->_previous = NULL;
				  if(shitBrick[moveList] != NULL)
					  printf("moveList: %d\n", moveList);
				  */
			  }
			  else
			  {

			    //if 64, ascend
			    if(moveList == 64)
			    {
			      ObjectHeader * moveObjHeader = (ObjectHeader*)moveSpace;
			      
			      ObjectHeader * cunt = (ObjectHeader*)shitBrick[moveList];

			      if(moveObjHeader->_objectSize <= cunt->_objectSize)
			      {
				 cunt->previous = moveSpace;
				 shitBrick[moveList] = moveSpace;
				 ((ObjectHeader *)shitBrick[moveList])->previous = NULL;
				 ((ObjectHeader *)shitBrick[moveList])->next = cunt;
			      }
			      else
			      {
				while(moveObjHeader->_objectSize > cunt->_objectSize && cunt->next != NULL)
				{
				  cunt = (ObjectHeader *)cunt->next;
				}

				if(cunt->next == NULL && moveObjHeader->_objectSize > cunt->_objectSize) //last in list
				{
				  cunt->next = moveObjHeader;
				  moveObjHeader->next = NULL;
				  moveObjHeader->previous = cunt;
				}
				else                             //stuck in between somewhere
				{
				  ((ObjectHeader*)cunt->previous)->next = moveObjHeader;
				  moveObjHeader->previous = cunt->previous;
				  cunt->previous = moveObjHeader;
				  moveObjHeader->next = cunt;
				}
			      }
			    }
			    else
			    {
			      ObjectHeader * cunt = (ObjectHeader *)shitBrick[moveList];
			      cunt->previous = moveSpace;
			      shitBrick[moveList] = moveSpace;
			      ((ObjectHeader *)shitBrick[moveList])->previous = NULL;
			      ((ObjectHeader *)shitBrick[moveList])->next = cunt;
			    }
			    
			  }
			  return returnSpace;
		  }

	    
		  
	  }
	  else //GET NEW SHIT FROM SBRK
	  {
		  if (totalSize < 16384)
		  {
		    
			  void * newSpace = getMemoryFromOS (16384);
			  ObjectHeader * oh = (ObjectHeader *)(newSpace);
			  oh->_objectSize = totalSize;
			  oh->_flags = ObjAllocated;
			  ObjectFooter * of = (ObjectFooter *)((char *)newSpace + totalSize - sizeof(ObjectFooter));
			  of->_objectSize = totalSize;
			  of->_flags = ObjAllocated;

			  
			  void * returnSpace = (char *)newSpace + sizeof(ObjectHeader);
			  
			  void * moveSpace = (char*)of + sizeof(ObjectFooter);
			  ObjectHeader * ohomo = (ObjectHeader *)(moveSpace);
			  ohomo->_objectSize = 16384-totalSize;
			  ohomo->_flags = ObjFree;
			  ObjectFooter * ofag = (ObjectFooter *)((char *)moveSpace + (16384-totalSize) - sizeof(ObjectFooter));
			  ofag->_objectSize = 16384-totalSize;
			  ofag->_flags = ObjFree;
			  int moveList = (ohomo->_objectSize >= 512? 64:((ohomo->_objectSize)>>3)-1);


			  if (shitBrick[moveList] == NULL)
			  {
				  shitBrick[moveList] = moveSpace;
				  ObjectHeader * cunt = (ObjectHeader *)shitBrick[moveList];
				  cunt->next = NULL;
				  cunt->previous = NULL;
				  if(shitBrick[moveList] != NULL)
				    printf("moveList: %d\n", moveList);

				
			  }
			  else
			  {
			    
			    
			    ObjectHeader * cunt = (ObjectHeader *)shitBrick[moveList];
			    cunt->previous = moveSpace;
			    shitBrick[moveList] = moveSpace;
			    ((ObjectHeader *) shitBrick[moveList])->previous = NULL;
			    ((ObjectHeader *) shitBrick[moveList])->next = cunt;
			    
			  }
			  return returnSpace;
		  }
		  else
		  {
		    /*SZ old part... copied and pasted new stuff from the 16k allocation
			  void * newSpace = getMemoryFromOS (totalSize);
			  return newSpace;
		    */
		    
			  void * newSpace = getMemoryFromOS (totalSize);
                          ObjectHeader * oh = (ObjectHeader *)(newSpace);
                          oh->_objectSize = totalSize;
                          oh->_flags = ObjAllocated;
                          ObjectFooter * of = (ObjectFooter *)((char *)newSpace + totalSize - sizeof(ObjectFooter));
                          of->_objectSize = totalSize;
                          of->_flags = ObjAllocated;

                          void * returnSpace = (char *)newSpace + sizeof(ObjectHeader);
			  return newSpace;
		  }
		  
		  
	  }
	  
  }

  // pthread_mutex_unlock(&mutex);
/*
  // Simple allocator always gets memory from the OS.
  void * mem = getMemoryFromOS( totalSize );

  // Get a pointer to the object header
  ObjectHeader * o = (ObjectHeader *) mem;

  // Store the totalSize. We will need it in realloc() and in free()
  o->_objectSize = totalSize;

  // Set object as allocated
  o->_flags = ObjAllocated;

  // Return the pointer after the object header.
  return (void *) (o + 1);
*/

}

void
Allocator::freeObject( void * ptr )
{
  //pthread_mutex_lock(&mutex);
  // Here you will return the object to the free list sorted by address and you will coalesce it
  // if possible.
  
  // Simple allocator does nothing.
  ObjectHeader * oh = (ObjectHeader *)((char*)ptr - sizeof(ObjectHeader));
  oh->_flags = ObjFree;

  size_t totalSize = oh->_objectSize;
  ObjectFooter * of = (ObjectFooter *)((char*)ptr + totalSize - sizeof(ObjectFooter));
  of->_flags = ObjFree;

  printf("FREE: objectSize %d\n", oh->_objectSize);
  //  size_t totalSize = (size + sizeof(ObjectHeader) + sizeof(ObjectFooter) + 7) & ~7;
  int list = (totalSize >= 512? 64:((totalSize)>>3)-1);

  
  
  if(shitBrick[list] == NULL)
  {
    shitBrick[list] = oh;
  }
  else
  {
    if(list != 64)
    {
      oh->next = shitBrick[list];
      ((ObjectHeader *)shitBrick[list])->previous = oh;
      oh->previous = NULL;
      shitBrick[list] = oh;
    }
    else
    {

      //temp segment to make it work since the commented code doesnt yet
      oh->next = shitBrick[list];
      ((ObjectHeader *)shitBrick[list])->previous = oh;
      oh->previous = NULL;
      shitBrick[list] = oh;

      /*
      //list 64 ascend
      ObjectHeader * cunt = (ObjectHeader *)shitBrick[list];
      if(oh->_objectSize <= cunt->_objectSize)
      {
	cunt->previous = oh;
	shitBrick[list] = oh;
	((ObjectHeader *)shitBrick[list])->previous = NULL;
	((ObjectHeader *)shitBrick[list])->next = cunt;
      }
      else
      {
	while(oh->_objectSize > cunt->_objectSize && cunt->next != NULL)
	{
	  cunt = (ObjectHeader *)cunt->next;
	}

	if(cunt->next == NULL && oh->_objectSize > cunt->_objectSize) //last in list
	{
	  cunt->next = oh;
	  oh->next = NULL;
	  oh->previous = cunt;
	}
	else                     //stuck in between somewhere
	{
	  ((ObjectHeader *)cunt->previous)->next = oh;
	  oh->previous = cunt->previous;
	  cunt->previous = oh;
	  oh->next = cunt;
	}
      }
      */
    }

  }


  //pthread_mutex_unlock(&mutex);
}

size_t
Allocator::objectSize( void * ptr )
{
  // Return the size of the object pointed by ptr. We assume that ptr is a valid obejct.
  ObjectHeader * o =
    (ObjectHeader *) ( (char *) ptr - sizeof(ObjectHeader) );

  // Substract the size of the header
  return o->_objectSize - sizeof(ObjectHeader);
}

void
Allocator::print()
{
  printf("\n-------------------\n");

  printf("HeapSize:\t%d bytes\n", _heapSize );
  printf("# mallocs:\t%d\n", _mallocCalls );
  printf("# reallocs:\t%d\n", _reallocCalls );
  printf("# callocs:\t%d\n", _callocCalls );
  printf("# frees:\t%d\n", _freeCalls );

  printf("\n-------------------\n");
}

void *
Allocator::getMemoryFromOS( size_t size )
{
  // Use sbrk() to get memory from OS
  _heapSize += size;
  
  return sbrk( size );
}

void
Allocator::atExitHandler()
{
  // Print statistics when exit
  if ( _verbose ) {
    print();
  }
}

//
// C interface
//

extern "C" void *
malloc(size_t size)
{
  Allocator::TheAllocator.increaseMallocCalls();
  
  return Allocator::TheAllocator.allocateObject( size );
}

extern "C" void
free(void *ptr)
{
  Allocator::TheAllocator.increaseFreeCalls();
  
  if ( ptr == 0 ) {
    // No object to free
    return;
  }
  
  Allocator::TheAllocator.freeObject( ptr );
}

extern "C" void *
realloc(void *ptr, size_t size)
{
  Allocator::TheAllocator.increaseReallocCalls();
    
  // Allocate new object
  void * newptr = Allocator::TheAllocator.allocateObject( size );

  // Copy old object only if ptr != 0
  if ( ptr != 0 ) {
    
    // copy only the minimum number of bytes
    size_t sizeToCopy =  Allocator::TheAllocator.objectSize( ptr );
    if ( sizeToCopy > size ) {
      sizeToCopy = size;
    }
    
    memcpy( newptr, ptr, sizeToCopy );

    //Free old object
    Allocator::TheAllocator.freeObject( ptr );
  }

  return newptr;
}

extern "C" void *
calloc(size_t nelem, size_t elsize)
{
  Allocator::TheAllocator.increaseCallocCalls();
    
  // calloc allocates and initializes
  size_t size = nelem * elsize;

  void * ptr = Allocator::TheAllocator.allocateObject( size );

  if ( ptr ) {
    // No error
    // Initialize chunk with 0s
    memset( ptr, 0, size );
  }

  return ptr;
}

extern "C" void 
checkHeap()
{
	// Verifies the heap consistency by iterating over all objects
	// in the free lists and checking that the next, previous pointers
	// size, and boundary tags make sense.
	// The checks are done by calling assert( expr ), where "expr"
	// is a condition that should be always true for an object.
	//
	// assert will print the file and line number and abort
	// if the expression "expr" is false.
	//
	// checkHeap() is required for your project and also it will be 
	// useful for debugging.
}

