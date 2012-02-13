
all: search-engine

# Build Search engine
search-engine: search-engine.o minihttpd.o array-dictionary.o \
	hash-dictionary.o avl-dictionary.o bsearch-dictionary.o 
	g++ -g -o search-engine search-engine.o minihttpd.o \
		  array-dictionary.o hash-dictionary.o avl-dictionary.o \
		  bsearch-dictionary.o -lnsl -lsocket

search-engine.o: search-engine.h search-engine.cpp
	g++ -g -c search-engine.cpp

# HTTP Server
minihttpd.o: minihttpd.cpp minihttpd.h
	g++ -g -c minihttpd.cpp

# Build dictionaries
array-dictionary.o: array-dictionary.h array-dictionary.cpp
	g++ -g -c array-dictionary.cpp

hash-dictionary.o: hash-dictionary.h hash-dictionary.cpp
	g++ -g -c hash-dictionary.cpp

avl-dictionary.o: avl-dictionary.h avl-dictionary.cpp
	g++ -g -c avl-dictionary.cpp

bsearch-dictionary.o: bsearch-dictionary.h bsearch-dictionary.cpp
	g++ -g -c bsearch-dictionary.cpp

# Used only for testing minihttpd independently
minihttpd: minihttpd.h minihttpd.cpp
	g++ -g -o minihttpd -DSTANDALONE minihttpd.cpp -lnsl -lsocket

clean:
	rm -f *.o minihttpd search-engine core
