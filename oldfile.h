#pragma once
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <array>
//#include "socket.h"

class File {

public:
    File();
    File(std::string filename);
    ~File();
    //int readData(struct Message& message);
    //void writeData(struct Message message);
    int readData(void);
    void writeData(void);
    void* getMapped();
    int getLength();

private:
    int fd_;
    void* p_;
};

// ------------------------------------


#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include "file.h"
#include <string.h>
#include <cerrno>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

// Clase File

File::File(){
}

File::File(std::string filename){
    
    fd_= open(filename.c_str(),  O_RDONLY | O_CREAT);

    lockf(fd_, F_LOCK, 0);

    if (fd_ < 0){
        throw std::system_error(errno, std::system_category(), "No se pudo abrir el fichero");
    }
    
    ftruncate(fd_, getLength());

    p = mmap(NULL, getLength(), PROT_READ, MAP_SHARED, fd, 0);

    lockf(fd_, F_ULOCK, 0);
    close(fd_);
}

File::~File(){
    munmap(p_, getLength());
}

int File::readData(void){
    
    int result = read(fd, p, getLength());
    
    if (result < 0){
        throw std::system_error(errno, std::system_category(), "No se pudo leer el mensaje");
    }

    return result;
    
    
    // int result = read(fd_, &message.text, message.text.size());
    // if (result < 0){
    //     throw std::system_error(errno, std::system_category(), "No se pudo leer el mensaje");
    // }

    // return result;
}

void File::writeData(void){
   
    int result = write(fd, p, getLength());
    
    if (result < 0){
        throw std::system_error(errno, std::system_category(), "No se pudo escribir el mensaje");
    }

    // int result = write(fd_, &message.text, message.text.size());
    // if (result < 0){
    //     throw std::system_error(errno, std::system_category(), "No se pudo escribir el mensaje");
    // }
}

void* File::getMapped() {
  return p_;
}

int File::getLength() {
  return lseek(fd_, 0, SEEK_END);
}