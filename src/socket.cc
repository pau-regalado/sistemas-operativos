#include "../include/socket.h"

Socket::Socket(const sockaddr_in &address) {
    fd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd_ < 0) {
        throw std::system_error(errno, std::system_category(),
                                "no se pudo crear el socket");
    }

   
        int result = bind(fd_, reinterpret_cast<const sockaddr *>(&address), sizeof(address));
        if (result < 0)  {
            throw std::system_error(errno, std::system_category()), "No se pudo ejecutar bind";
        }
}

Socket::~Socket(){
    close(fd_);
}

int Socket::get_fd(void){
  return fd_;
}



sockaddr_in make_ip_address(int port, const std::string &ip_address)
{

    // Dirección del socket local
    sockaddr_in local_address{};        // Así se inicializa a 0, como se recomienda
    local_address.sin_family = AF_INET; // Pues el socket es de dominio AF_INET

    if (ip_address.empty()) {
        local_address.sin_addr.s_addr = INADDR_ANY;
    }
    else {
        inet_aton(ip_address.c_str(), &local_address.sin_addr);
    }
    local_address.sin_port = htons(port);

    return local_address;
    
}

void Socket::send_to(const Message& message, const sockaddr_in& address) {

    int result = sendto(fd_, &message, sizeof(message), 0, reinterpret_cast<const sockaddr*>(&address), sizeof(address));
    
    if (result < 0)   {
        throw std::system_error(errno, std::system_category(), "Error enviando mensaje");
    }
}

int Socket::receive_from(sockaddr_in &address, std::string& respuesta){
    // Recibir un mensaje del socket remoto
    //std::cout << message.text.data() << std::endl;sizeof(address)
    socklen_t src_len;
    Message buffer;
    int result = recvfrom(fd_, &buffer, sizeof(buffer), 0, nullptr, nullptr); //(socklen_t) sizeof(address));
    if (result < 0) {
        throw std::system_error(errno, std::system_category(), "No se recibió el mensaje");
    }

    std::string s(buffer.text.data(), buffer.size);
    respuesta = s;
    //std::cout << "Se leyeron " << buffer.size << " bytes del mensaje. Resultado:\n" << s << std::endl;
    //std::cout << s;
    return buffer.size;
}
