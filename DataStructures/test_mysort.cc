
#include "mysort.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

int a[] = {45, 23, 78, 12, 100, 1, 100, 34, 90, 78 };

int compareInt( void * a, void * b )
{
  return *(int*)a - *(int*) b;
}

void
testWithInt()
{
  int i;
  int nelements = sizeof(a)/sizeof(int);

  printf("--- Numbers ----\n");
  for ( i = 0; i < nelements; i++ ) {
    printf("a[%d]=%d\n", i, a[i] );
  }

  mysort( nelements, sizeof(int), a, 1, compareInt );
  
  printf("---- Numbers after Sorting ----\n");
  for ( i = 0; i < nelements; i++ ) {
    printf("a[%d]=%d\n", i, a[i] );
  }

  // Test that numbers are ordered
  for ( i = 0; i < nelements -1 ; i++ ) {
    assert( a[i] <= a[i+1] );
  }

  printf( "testWithint OK\n");
}

struct Student {
  const char * name;
  int grade;
};

Student students[] = {
  {"Rachael", 8 },
  {"Monica", 9},
  {"Phoebe", 10},
  {"Joey", 10},
  {"Ross", 8},
  {"Chandler", 8}
};

int compareStudentByGrade( void * a, void * b )
{
  Student * sa = (Student *) a;
  Student * sb = (Student *) b;

  // Sort by grade
  if ( sa->grade > sb->grade ) {
    return 1;
  }
  else if ( sa->grade == sb->grade ) {
    // Sort alphabetically if equal
    return strcmp( sa->name, sb->name );
  }

  return -1;
}

int compareStudentByName( void * a, void * b )
{
  Student * sa = (Student *) a;
  Student * sb = (Student *) b;

  // Sort alphabetically
  return strcmp( sa->name, sb->name );
}

void
testWithStudents()
{
  int i;
  int nelements = sizeof(students)/sizeof(Student);

  printf("----- Students ----\n");
  
  printf("Before\n");
  for ( i = 0; i < nelements; i++ ) {
    printf("%d:%-20s %2d\n", i, students[i].name, students[i].grade );
  }

  printf("----- Students Sorted by Grade ----\n");

  mysort( nelements, sizeof(Student), students, 1, compareStudentByGrade );
  
  for ( i = 0; i < nelements; i++ ) {
    printf("%d:%-20s %2d\n", i, students[i].name, students[i].grade );
  }

  // Test that students are ordered by grade
  for ( i = 0; i < nelements -1 ; i++ ) {
    assert( students[i].grade <= students[i+1].grade );
  }

  printf("----- Students Sorted by Name ----\n");

  mysort( nelements, sizeof(Student), students, 1, compareStudentByName );
  
  for ( i = 0; i < nelements; i++ ) {
    printf("%d:%-20s %2d\n", i, students[i].name, students[i].grade );
  }
 
  // Test that students are ordered by name
  for ( i = 0; i < nelements -1 ; i++ ) {
    assert( strcmp(students[i].name, students[i+1].name) <= 0 );
  }

  printf("----- Students Sorted by Name in Descending order ----\n");

  mysort( nelements, sizeof(Student), students, 0, compareStudentByName );
  
  for ( i = 0; i < nelements; i++ ) {
    printf("%d:%-20s %2d\n", i, students[i].name, students[i].grade );
  }
 
  // Test that students are ordered by name in descending order
  for ( i = 0; i < nelements-1 ; i++ ) {
    assert( strcmp(students[i].name, students[i+1].name) >= 0 );
  }

  printf( "testWithStudents OK\n");
}

int main( int argc, char ** argv )
{
  testWithInt();
  testWithStudents();

  printf(">>> test_mysort Succeeded...\n");
}
