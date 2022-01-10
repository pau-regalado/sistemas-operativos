INCLUDE=./include/
SRC=./src/

all: funciones emisor receptor file 

file: $(INCLUDE)file.h
	g++ $(SRC)file.cc -c

socket: $(SRC)socket.cc $(INCLUDE)socket.h
	g++ $(SRC)socket.cc -c

funciones: $(SRC)funciones.cc $(INCLUDE)funciones.h
	g++ $(SRC)funciones.cc -c

emisor: socket file
	g++ $(SRC)emisor.cc -o socketemisor socket.o file.o funciones.o

receptor: socket
	g++ $(SRC)receptor.cc -o socketreceptor socket.o file.o funciones.o

clean:
	rm *.o socketreceptor socketemisor