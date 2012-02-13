CC = g++ -g

all: test1 test2 test3 brk maxheap stress testcalloc testrealloc testfree MyMalloc.so

MyMalloc.so: MyMalloc.cc
	$(CC) -g -c -fPIC MyMalloc.cc
	g++ -shared -lgcc -Wl -o MyMalloc.so MyMalloc.o

test1: test1.cc MyMalloc.so
	$(CC) -o test1 test1.cc MyMalloc.so

brk: brk.c
	$(CC) -o brk brk.c MyMalloc.so

maxheap: maxheap.c
	$(CC) -o maxheap maxheap.c MyMalloc.so

stress: stress.c
	$(CC) -o stress stress.c MyMalloc.so

test2: test2.cc MyMalloc.so
	$(CC) -o test2 test2.cc MyMalloc.so

test3: test3.cc MyMalloc.so
	$(CC) -o test3 test3.cc MyMalloc.so -lthread

testcalloc: testcalloc.c
	$(CC) -o testcalloc testcalloc.c MyMalloc.so

testrealloc: testrealloc.c
	$(CC) -o testrealloc testrealloc.c MyMalloc.so

testfree: testfree.c
	$(CC) -o testfree testfree.c MyMalloc.so

clean:
	rm -f *.o test1 test2 test3 test4 test5 MyMalloc.so core a.out



