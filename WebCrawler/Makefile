
all: gethttp

gethttp: gethttp.cpp openhttp.cpp
	g++ -o gethttp -g gethttp.cpp openhttp.cpp -lnsl -lsocket -g

clean:
	rm -f *.o gethttp core
