#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include "socket.h"
#include "file.h"

int protected_main(int argc, char* argv[]) {
    std::string ip_address;
    int port = 0;
    
    sockaddr_in direccion = make_ip_address(port,ip_address);

    std::string ip_destino= "127.0.0.2";
    int port_destino = 5001;
    sockaddr_in remote_address = make_ip_address(port_destino,ip_destino);
    
    Socket emisor(direccion);

    Message message;
    File f(argv[1]);

    std::cout << "Enviando archivo: " << argv[1] << std::endl;

    int leidos = 0;

    bool fin = false;

    do{

      leidos = read(f.GetFd(), &message, sizeof(message.text));
      message.size = leidos;

      // Se envia un ultimo mensaje vacio, para que el receptor 
      // sepa que ya se ha acabado el envio
      if (leidos == 0)
        fin = true;

      std::string message_text(message.text.data(),leidos);
      message_text.copy(message.text.data(), message.text.size() - 1, 0);
      emisor.send_to(message, remote_address); 

    }while (leidos >= 0 && !fin);

    std::cout << std::endl << "ARCHIVO ENVIADO CORRECTAMENTE, HASTA LUEGO LUCA" << std::endl;

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

