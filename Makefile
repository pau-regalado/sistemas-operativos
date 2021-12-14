all: emisor receptor file

file: file.h
	g++ file.cc -c

socket: socket.cc socket.h
	g++ socket.cc -c

emisor: socket file
	g++ emisor.cc -o socketemisor socket.o file.o

receptor: socket
	g++ receptor.cc -o socketreceptor socket.o file.o

clean:
	rm *.o socketreceptor socketemisor