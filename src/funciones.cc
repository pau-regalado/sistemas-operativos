#include "../include/funciones.h"

int receive_file(std::string file_name, std::string ip_dest, int p_dest){

  sockaddr_in remote_address = make_ip_address(p_dest,ip_dest);
  Socket receptor(remote_address);

  sockaddr_in estructura{};
  // fichero donde se guardara
  
  File f(file_name);

  std::string received_data;
  int result;
  do{
    result = receptor.receive_from(estructura, received_data);
    //std::cout << "Se han leido " << result << " bytes" << std::endl;
    if (result > 0)
      write(f.GetFd(),received_data.c_str(), result);
  }while(result > 0);

  std::cout << std::endl << "FIN DEL ARCHIVO, HASTA LUEGO LUCA" << std::endl;

  return 0;
}

int receive_mapped_file(std::string file_name, std::string ip_dest, int p_dest){

  sockaddr_in remote_address = make_ip_address(p_dest,ip_dest);
  Socket receptor(remote_address);

  sockaddr_in estructura{};
  // fichero donde se guardara
  
  File f(file_name);

  std::string received_data;
  int result;
  do{
    result = receptor.receive_from(estructura, received_data);
    //std::cout << "Se han leido " << result << " bytes" << std::endl;
    if (result > 0)
      write(f.GetFd(),received_data.c_str(), result);
  }while(result > 0);

  std::cout << std::endl << "FIN DEL ARCHIVO, HASTA LUEGO LUCA" << std::endl;

  return 0;
}

int send_file(std::string file_name, std::string ip_dest, int p_dest){

    std::string ip_address;
    int port = 0;
    //Direccion del emisor
    sockaddr_in direccion = make_ip_address(port,ip_address);
    Socket emisor(direccion);
    //Direccion del receptor
    sockaddr_in remote_address = make_ip_address(p_dest,ip_dest);
    
    Message message;
    File f(file_name);

    std::cout << "Enviando archivo: " << file_name << std::endl;

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