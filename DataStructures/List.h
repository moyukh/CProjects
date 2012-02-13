
struct ListNode {
  int _value;
  ListNode * _next;
};

class List {
public:
  //Head of list
  ListNode * _head;
  
  void insertSorted( int val ) ;
  int lookup( int _value );
  int remove( int val );
  void print();
  List();
};

