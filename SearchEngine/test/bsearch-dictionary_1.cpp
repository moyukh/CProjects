
// Implementation of a dictionary using an array and binary search
// It will inherit from the ArrayDictionary

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "bsearch-dictionary.h"

// Constructor
BinarySearchDictionary::BinarySearchDictionary()
{
  // Add needed code
  index = 0;
  length = 5000;
  array = new DictMember[length];
}

inline int leftchild(int parent){
  return 2*parent+1;
}
inline int rightchild(int parent){
  return 2*parent+2;
}
inline int parent(int child){
  return(child -1)/2;
}

bool
BinarySearchDictionary::addRecord( KeyType key, DataType record)
{
  sorted = false;
  // Add needed code
  URLNumList * temp= new URLNumList();
  bool found = false;
  for (int i = 0; i<index; i++)
  {
	  if (strcmp(key, array[i].word) == 0)
	  {
	    //printf("%d current\n", array[i].list -> _num);
		  temp = (URLNumList*)record;
		  
		  
		  
		  if (temp -> _num <= array[i].list -> _num) //insert before lesser value
		  {
			  temp -> _next = array[i].list;
			  array[i].list = temp;
		  }
		  else
		  {
			  URLNumList *iterator = array[i].list;
			  while(iterator->_next != NULL && iterator->_next->_num < temp -> _num) //insert after higher value
			  {
				  iterator = iterator->_next;
			  }
			  temp->_next = iterator->_next;
			  iterator->_next = temp;
			  
		  }
		  
		  
		  found = true;
		  /*
		  int j = array[i].list -> _num;
		  printf("%d inserted\n", j);
		  j = array[i].list -> _next -> _num;
		  printf("%d there\n", j);*/
		  return found;
	  }
		  
  }
  
  if (!found)
  {
    if(index>=length-1){
      //printf("array[index]")"
      length *=2;
      temparray = new DictMember[length];
      //memcpy(temparray,array, sizeof(DictMember*)*length/2);
      for(int y = 0; y<length;y++)
	temparray[y] = array[y];
      delete(array);
      array = temparray;
    }
    array[index].word = strdup(key);
    
    array[index].list = (URLNumList*)record;
	  
    //int j = array[index].list -> _num;
    // printf("%d inserted\n", j);
    index++;
  }    
  return false;
}

// Find a key in the dictionary and return corresponding record or NULL
DataType
BinarySearchDictionary::findRecord( KeyType key)
{
  // Ue binary search
  // Add needed code
  if(sorted == false)
    sort();
  int low = 0;
  int high = index-1;
  while(high>=low){
    //printf("high = %d. %s, low = %d. %s\n",high,array[high].word,low,array[low].word);
    int mid = (high + low)/2;
    if(strcmp(array[mid].word,key) == 0){
      return array[mid].list;
    }
    else if(strcmp(array[mid].word,key)>0){
      high = mid -1;
    }
    else{
      low = mid + 1;
    }
  }
  return NULL;
}

// Sort array using heap sort.
void
BinarySearchDictionary::sort()
{
  // Add needed code
  sorted = true;
  DictMember *heap = new DictMember[length];
  int maxsize; //probably equals length
  int last = 0;
  //insert the elements
  for(int i = 0; i<index; i++){ //do heap insert function
    heap[last].word = array[i].word;
    heap[last].list = array[i].list;
    last++;
    int ichild = last - 1;
    int iparent = parent(ichild);
    while(ichild>0){
      if(strcmp(heap[ichild].word,heap[iparent].word) > 0){
	//no swapping necessay
	break;
      }
      //may not work, may have to memcopy
      DictMember temp = heap[ichild];
      heap[ichild] = heap[iparent];
      heap[iparent] = temp;
      ichild = iparent;
      iparent = parent(ichild);
    }//end while
  } //this should have the  heap
  //remove minkey and insert into array
  for(int i = 0; i<index; i++){
    //printf("%s ",heap[0].word);
    array[i].word = heap[0].word;
    array[i].list = heap[0].list;
    heap[0] = heap[last-1];
    last--;
    int iparent = 0;
    int ileft = leftchild(iparent);
    int iright = rightchild(iparent);
    while(ileft<last){
      int iminchild = ileft;
      if(iright<last){
	if(strcmp(heap[iright].word,heap[ileft].word)<0){
	  iminchild = iright;
	}
      }

      if( strcmp(heap[iparent].word,heap[iminchild].word)>0){
	//we need to swap
	DictMember temp = heap[iparent];
	heap[iparent] = heap[iminchild];
	heap[iminchild] = temp;
      }
      else{
	break;
      }
      //Update iparent, ileft, iright
      iparent = iminchild;
      ileft = leftchild(iparent);
      iright = rightchild(iparent);
    } //end while
  }//end for
  
    for(int i = 0; i<index; i++){
      //printf("%d. %s",i,array[i].word);
    URLNumList* current = array[i].list;
    while(current!=NULL){
      //printf(" %d",current->_num);
      current= current->_next;
    }
    // printf("\n");
  }


}

