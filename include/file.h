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
#include <cmath>

const unsigned SEND_CAPACITY = 64 * pow(2, 10) - 8;

class File {
  public:

    File(std::string filename, int size = 0);
    ~File();

    int GetFd(void);
    void* get_mmapped(void);
    unsigned get_length(void);
    std::string get_name(void);

  private:
    //void create_socket(const sockaddr_in& address);
    int fd_;
    int length_;
    void* mmapped_;
    std::string name_;
};