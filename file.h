#pragma once
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <string.h>
#include <cerrno>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

class File {
  public:

    File(std::string filename);
    ~File();

    int GetFd(void);

  private:
    //void create_socket(const sockaddr_in& address);
    int fd_;
};