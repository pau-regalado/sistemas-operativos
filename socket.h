#pragma once
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <array>


struct Message {

    std::array<char, 1024> text;
    int size = 0;

};

class Socket {
  public:
    Socket(const sockaddr_in& address);
    ~Socket();
    
    void send_to(const Message& message, const sockaddr_in& address);
    //void send_to(const std::string& message, const sockaddr_in& address);
    int receive_from(sockaddr_in& address);//, std::string& respuesta);


private:
    //void create_socket(const sockaddr_in& address);
    int fd_;
};



//constructir de sockaddr_in
sockaddr_in make_ip_address(int port, const std::string& ip_address = std::string());

