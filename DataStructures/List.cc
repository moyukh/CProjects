
//
// Implement the List class
//

#include <stdio.h>
#include "List.h"

//
// Inserts a new element with value "val" in
// ascending order.
//
void
List::insertSorted( int val )
{
  // Complete procedure
  ListNode* node = new ListNode;
  
  node->_value = val;
  if (_head == NULL) //create list if none exists
  {
	  node->_next = NULL;
	  _head = node;
	  return;
  }
  else if (val <= _head->_value) //insert before lesser value
  {
	  node->_next = _head;
	  _head = node;
  }
  else
  {
	  ListNode* iterator = _head;
	  while(iterator->_next != NULL && iterator->_next->_value < val) //insert after higher value
	  {
		  iterator = iterator->_next;
	  }
	  node->_next = iterator->_next;
	  iterator->_next = node;
	  
  }
  
  
}

// Removes an element with value "val" from List
// Returns 0 if succeeds or -1 if it fails
int 
List:: remove( int val )
{
  // Complete procedure
  ListNode* iterator = _head;
  if (iterator->_value == val)
  {
	  _head = iterator->_next; //move head if value at start
	  return 0;
  }
  do
  { 
	  if (iterator->_next->_value == val) //find value
	  {
		  
		  ListNode* temp = iterator->_next->_next; //remove from list
		  iterator->_next = temp;
		  
		  return 0;
	  }
	  iterator = iterator->_next;
  }while (iterator->_next != NULL);
  
  return -1; //value not found
}

// Prints The elements in the list. 
void
List::print()
{
  // Complete procedure 
  ListNode* current = _head; //print values
  while (current != NULL)
  {
	  printf("%d ", current->_value);
	  current = current -> _next;
  }
}

//
// Returns 0 if "value" is in the list or -1 otherwise.
//
int
List::lookup(int val)
{ // Complete procedure 
  ListNode* current = _head;
  while (current != NULL)
  {
	  if (current->_value == val) //value found
		  return 0;
	  current = current -> _next;
  }
 
  return -1; //value not found
}

//
// List constructor
//
List::List()
{
  // Complete procedure
  _head = NULL;
}

