
// Implementation of a dictionary using an AVL tree
// AVL Trees are balanced binary trees 

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "avl-dictionary.h"


// Constructor
AVLDictionary::AVLDictionary()
{
  // Add needed code
  root = NULL;
}

// Add a record to the dictionary. Returns false if key already exists
bool
AVLDictionary::addRecord( KeyType key, DataType record)
{
  
  //printf("start\n");
  URLNumList *temp = new URLNumList();
  DictMember *current = root;
  DictMember *previous = NULL;
  while(current !=NULL){
    //printf("while,key = %s, current word = %s\n",key,current->word);
    if(strcmp(key,current->word) == 0){ // word exist add to the URLNUMList
      //essentially follows array-dict's way with array[i] replaced by current
      temp = (URLNumList*)record;
      if (temp -> _num <= current->list -> _num){ //insert before lesser value
	temp -> _next = current->list;
	current->list = temp;
      }
      else{
	URLNumList *iterator = current->list;
	while(iterator->_next != NULL && iterator->_next->_num < temp -> _num){
	  //insert after higher value	  
	  iterator = iterator->_next;
	}
	temp->_next = iterator->_next;
	iterator->_next = temp;
      }
      return true; //no restructuring needed
    }//end if(strcmp(key,current->word) == 0)
    else if( strcmp(key,current->word) < 0 ){
      //printf("left");
      //printf("%d, %d",strcmp("near","nancy"),strcmp(key,current->word));
      previous = current;
      current = current ->left;
    }
    else if( strcmp(key,current->word) > 0){ 
      //just change to else for optimization
      //printf("right");
      previous = current;
      current = current ->right;
    }
  } // end while(current !=NULL){ 
  //need to create new node
  //printf("newnode\n");
  DictMember * n = new DictMember();
  n->height = 1;
  n->word = strdup(key);
  n->list = (URLNumList*)record;
  n->left = NULL;
  n->right = NULL;
  n->parent = previous;
  if(previous == NULL){
    //tree is empty, insert at root
    root = n;
    //printf("insert root\n");
  }
  else{
    if(strcmp(key,previous->word)<0){
      previous->left = n;
    }
    else{
      previous->right = n;
    }
  }
  //Adjust the height of the tree
  DictMember * m = n->parent;
  while(m != NULL){
    //get maximum height of the left and right child
    int maxheight = 0;
    if(m->left!=NULL){
      maxheight = m->left->height;
    }
    if(m->right != NULL && maxheight < m->right->height){
      maxheight = m->right->height;
    }
    m->height = 1+maxheight;
    m=m->parent;
  }
  //Restructure
  restructure(n);
  return false;
}

void AVLDictionary::restructure(DictMember *n){
  //Rebalance tree starting at n
  //Find z=parent of the first unbalanced subtree
  DictMember *z = n->parent;
  while(z!=NULL){
    //Find parent of unbalanced subtree
    int hleft = 0;
    if(z->left != NULL){
      hleft = z->left->height;
    }
    int hright = 0;
    if(z->right != NULL){
      hright = z ->right->height;
    }
    //At this point we recomputued the height of z in case it was modified during this restructuring
    if(hleft>hright){
      z->height = hleft +1;
    }
    else{
      z->height = hright +1;
    }
    int hdiff = hright - hleft;
    if(hdiff<0)
      hdiff = -hdiff;
    //test if subtree rooted by z is unbalanced
    if(hdiff<=1){
      //subtree is balanced
      z=z->parent;
      continue;
    }
    //subtree rooted by z is unbalanced. Find y: child with the largest height
    DictMember *y = NULL;
    int maxh = 0;
    if(z->left!=NULL){
      y=z->left;
      maxh = y->height;
    }
    if(z->right!=NULL && maxh < z->right->height){
      y = z->right;
    }
    assert(y!=NULL);
    //Find x: child of y with the largest height
    DictMember *x = NULL;
    maxh = 0;
    if(y->left != NULL){
      x = y->left;
      maxh = y->height;
    }
    if(y->right != NULL && maxh<y->right->height){
      x=y->right;
    }
    assert(x!=NULL);
    //Now we have x,y,z. We need to relabel x,y,z as a,b,c according to inorder
    //traversal
    DictMember *a, *b, *c, *t0, *t1, *t2, *t3;
    if(z->right==y){
      //case 1 or 2
      if(y->right == x){
	//case 1
	a = z; b = y; c = x;
	t0 = z->left; t1 = y->left; t2 = x->left; t3 = x->right;
      }
      else{
	//case 2
	a=z; b=x; c=y;
	t0=z->left; t1=x->left; t2 = x->right; t3 = y->right;
      }
    }
    else{
      //case 3 or 4
      if(y->left == x){
	//case 3
	a=x; b=y; c=z;
	t0=x->left; t1=x->right; t2=y->right; t3=z->right;
      }
      else{
	//case 4
	a=y; b=x; c=z;
	t0=y->left; t1=x->left; t2 = x->right; t3 = z->right;
      }
    }
    //We have a, b, and c. Do the Rotation
    DictMember *p = z->parent;
    if(p!=NULL){
      //connect to parent of z
      if(p->left == z){
	//connect to the left of p
	p->left = b;
      }
      else{
	p->right = b;
      }
    }
    else{
      //make b the new root
      root = b;
    }
    b->parent = p;
    b->left = a;
    b->right = c;
    a->parent = b;
    a->left = t0;
    a->right = t1;
    c->parent = b;
    c->left = t2;
    c->right = t3;
    //connect parents of t1, t2, t3, t4 pointers
    if(t0 != NULL){
      t0->parent = a;
    }
    if(t1 != NULL){
      t1->parent = a;
    }
    if(t2 != NULL){
      t2->parent = c;
    }
    if(t3 != NULL){
      t3->parent = c;
    }
    //update height of a
    int maxheight = 0;
    if(a->left != NULL){
      maxheight = a->left->height;
    }
    if(a->right != NULL && a->right->height>maxheight){
      maxheight = a->right->height;
    }
    a->height = 1+maxheight;
    //Update height of c
    maxheight = 0;
    if(c->left!=NULL){
      maxheight = c->left->height;
    }
    if(c->right != NULL && c->right->height>maxheight){
      maxheight=c->right->height;
    }
    c->height = 1+maxheight;
    //Update height of b
    maxheight = 0;
    if(b->left != NULL){
      maxheight = b->left->height;
    }
    if(b->right != NULL && b->right->height>maxheight){
      maxheight = b->right->height;
    }
    b->height = 1+maxheight;
    //We are done fixing the  subtree go up to the parent fixing the parent
    //until reaching the root update z looking for another unbalanced subtree
    z=p;
  }//end while
}//end restructure

// Find a key in the dictionary and return corresponding record or NULL
DataType
AVLDictionary::findRecord( KeyType key)
{
  //printf("start findRecord\n");
  //printf("key = %s\n",key);
  DictMember *current = root;
  while(current!=NULL){
    //printf("current word = %s, height = %d\n", current->word,current->height);
    if(strcmp(key,current->word) < 0)
      current = current->left;
    else if(strcmp(key,current->word) > 0)
      current = current->right;
    else{
      //word found
      return current->list;
    }
  }
  // Add needed code
  return NULL;
}
