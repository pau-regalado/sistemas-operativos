#include "file.h"

File::File(std::string filename){

  fd_= open(filename.c_str(), O_RDONLY);

  if (fd_ < 0){
    throw std::system_error(errno, std::system_category(), "No se pudo abrir el fichero");
  }
}

File::~File(){
  close(fd_);
}

int File::GetFd(void){
  return fd_;
}