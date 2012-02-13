//
// CS251 Data Structures
// Hash Table
//

#include <assert.h>
#include <stdlib.h>
#include <string.h>

// Each list entry stores data
template <typename Data>
struct ListEntry {
  Data _data;
  ListEntry * _next;
};

// This is a Hash table that maps string keys to objects of type Data
template <typename Data>
class ListGeneric {
 public:
  ListEntry<Data> * _head;
  
  ListGeneric();
  void insert(Data data);
  bool remove(Data &data);
};

template <typename Data>
ListGeneric<Data>::ListGeneric()
{
  _head = NULL;
}

template <typename Data>
void ListGeneric<Data>::insert(Data data)
{
  ListEntry<Data> * e = new ListEntry<Data>;
  e->_data = data;
  e->_next = _head;
  _head = e;
}

template <typename Data>
bool ListGeneric<Data>::remove(Data &data)
{
  if (_head==NULL) {
    return false;
  }
  
  ListEntry<Data> * e = _head;
  data = e->_data;
  _head = e->_next;
  delete e;
  return true;
}

template <typename Data>
class ListGenericIterator {
  ListEntry<Data> *_currentEntry; // Points to the current node
  ListGeneric<Data> * _list;
 public:
  ListGenericIterator(ListGeneric<Data> * list);
  bool next(Data & data);
};

template <typename Data>
ListGenericIterator<Data>::ListGenericIterator(ListGeneric<Data> * list)
{
  _list = list;
  _currentEntry = _list->_head;
}

template <typename Data>
bool ListGenericIterator<Data>::next(Data & data)
{
  if (_currentEntry == NULL) {
    return false;
  }

  data = _currentEntry->_data;
  _currentEntry = _currentEntry->_next;

  return true;
}
