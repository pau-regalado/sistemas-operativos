#pragma once
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <exception>
#include <thread>
#include <pthread.h>
#include <dirent.h>
#include <vector>
#include <utility>
#include <atomic>
#include <csignal>

#include "socket.h"
#include "file.h"

int receive_list(Socket& local, sockaddr_in& address, std::string& respuesta);

int peticion_handler(std::exception_ptr& eptr, Socket& local);

int commands_processor(std::exception_ptr& eptr, Socket& local);

void list_dir(const std::string& name, std::string& res);
void read_directory(const std::string& name, std::vector<std::string>& v);

void show_menu (void);

int receive_file(std::string file_name, std::string ip_dest, 
                 int p_dest);

int receive_mapped_file(std::string file_name, std::string ip_dest,
                        int p_dest);
int send_file(std::string file_name, std::string ip_dest, 
              int p_dest);

int send_mapped_file(std::string file_name, std::string ip_dest, 
                     int p_dest);
