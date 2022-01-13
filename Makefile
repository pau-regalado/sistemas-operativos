INCLUDE=./include/
SRC=./src/

all: funciones viewnet emisor receptor file 

file: $(INCLUDE)file.h
	g++ $(SRC)file.cc -c -pthread

socket: $(SRC)socket.cc $(INCLUDE)socket.h
	g++ $(SRC)socket.cc -c -pthread

funciones: $(SRC)funciones.cc $(INCLUDE)funciones.h
	g++ $(SRC)funciones.cc -c -pthread

emisor: socket file
	g++ $(SRC)emisor.cc -o socketemisor socket.o file.o funciones.o -pthread

receptor: socket
	g++ $(SRC)receptor.cc -o socketreceptor socket.o file.o funciones.o -pthread

viewnet: socket file 
	g++ $(SRC)ViewNet.cc -o viewnetexec socket.o file.o funciones.o -pthread

clean:
	rm *.o socketreceptor socketemisor