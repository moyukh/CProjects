CC = /opt/SUNWspro/bin/CC -g

#AS=/usr/ccs/bin/as

all: test1 test2 test3 MyMalloc.so runtests

runtests: runtest1 runtest2 runtest3 runtest4

clearcounter:
	rm -f npassed
	rm -f nfailed

MyMalloc.so: MyMalloc.cc
	$(CC) -c MyMalloc.cc -o MyMalloc.o
	ld -G -o MyMalloc.so MyMalloc.o

test1: test1.cc MyMalloc.so
	$(CC) -o test1 test1.cc MyMalloc.so

test2: test2.cc MyMalloc.so
	$(CC) -o test2 test2.cc MyMalloc.so

test3: test3.cc MyMalloc.so
	$(CC) -o test3 test3.cc MyMalloc.so -lthread

runtest1:
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:`pwd` && export LD_LIBRARY_PATH && \
	echo "--- Running test1 ---" && \
	test1

runtest2:
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:`pwd` && export LD_LIBRARY_PATH && \
	echo "--- Running test2 ---" && \
	test2

runtest3:
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:`pwd` && export LD_LIBRARY_PATH && \
	echo "--- Running test3 ---" && \
	test3

runtest4: MyMalloc.so
	echo && \
	echo "---- Running test4 (run cc with your malloc) ----" && \
	LD_PRELOAD=`pwd`/MyMalloc.so && \
	export LD_PRELOAD && \
	/opt/SUNWspro/bin/cc hello.c && \
	echo ">>> Test4 passed\n" || \
	echo "*** Test4 failed\n"

clean:
	rm -f *.o test1 test2 test3 test4 test5 MyMalloc.so core a.out
