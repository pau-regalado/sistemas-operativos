#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include "socket.h"
#include <exception>
#include <thread>


int protected_main(int argc, char* argv[]) {

    std::string ip_destino= "127.0.0.2";
    int port_destino = 5001;
    sockaddr_in remote_address = make_ip_address(port_destino,ip_destino);
    Socket receptor(remote_address);

    sockaddr_in estructura{};
    int result;
    do{
       result = receptor.receive_from(estructura);
    }while(result > 0);

    std::cout << std::endl << "FIN DEL ARCHIVO, HASTA LUEGO LUCA" << std::endl;

    return 0;
}


int main(int argc, char* argv[]) {
    try {

        return protected_main(argc, argv);

    } catch(std::bad_alloc& e) {
        std::cerr << "mytalk" << ": memoria insuficiente\n";
        return 1;
    } catch(std::system_error& e) {
        std::cerr << "mitalk" << ": " << e.what() << '\n';
        return 2;
    } catch (...) {
        std::cout << "Error desconocido\n";
        return 99;
    }
}