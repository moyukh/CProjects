#Use GNU compiler
cc = gcc -g
CC = g++ -g

all: shell #cat_grep ctrl-c regular

lex.yy.o: shell.l 
	/usr/ccs/bin/lex shell.l
	$(cc) -c lex.yy.c

y.tab.o: shell.y
	/usr/ccs/bin/yacc -d shell.y
	$(CC) -c y.tab.c

command.o: command.cc
	$(CC) -c command.cc

shell: y.tab.o lex.yy.o command.o
	$(CC) -o shell lex.yy.o y.tab.o command.o -ll -lgen

#cat_grep: cat_grep.cc
#	$(CC) -o cat_grep cat_grep.cc

#ctrl-c: ctrl-c.cc
#	$(CC) -o ctrl-c ctrl-c.cc

#regular: regular.cc
#	$(CC) -o regular regular.cc -lgen

clean:
	rm -f lex.yy.c y.tab.c  y.tab.h *.*~ shell ctrl-c regular cat_grep *.o

