#pragma once
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include "socket.h"
#include "file.h"
#include <exception>
#include <thread>

int receive_file(std::string file_name, std::string ip_dest, 
                 int p_dest);

int receive_mapped_file(std::string file_name, std::string ip_dest,
                 int p_dest);
int send_file(std::string file_name, std::string ip_dest, 
              int p_dest);
