#include "../include/file.h"

File::File(std::string filename){ //, int size){

  fd_= open(filename.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd_ < 0){
    throw std::system_error(errno, std::system_category(), "No se pudo abrir el fichero");
  }
  lockf(fd_, F_LOCK, 0);
  /*

  length_ = lseek(fd_, 0, SEEK_END); 
  
  mmapped_ = mmap(NULL, length_, 
                  PROT_READ | PROT_WRITE, 
                  MAP_SHARED, fd_, 0);*/
}

File::~File(){

  //munmap(mmapped_, length_);
  lockf(fd_, F_ULOCK, 0);
  if (close(fd_) == 0)
    fd_ = 0;
}

int File::GetFd(void){
  return fd_;
}

void* File::get_mmapped(void) {
  return mmapped_;
}

unsigned File::get_length(void){
  return length_;
} 