#include "../include/funciones.h"

int receive_file(std::string file_name, std::string ip_dest, int p_dest){

  sockaddr_in remote_address = make_ip_address(p_dest,ip_dest);
  Socket receptor(remote_address);
  
  //Obtenemos los metadatos del fichero
  sockaddr_in metaestructura{};
  std::string received_metadata;
  //Primero recibimos el tamaño
  
  receptor.receive_from(metaestructura, received_metadata);
  int sizef = stoi(received_metadata);
  //Luego el nombre
  receptor.receive_from(metaestructura, received_metadata);
  //fichero donde se guardara
  File file(received_metadata, sizef);

  std::string received_data;
  sockaddr_in estructura{};
  int result;
  do{
    result = receptor.receive_from(estructura, received_data);
    //std::cout << "Se han leido " << result << " bytes" << std::endl;
    if (result > 0)
      write(file.GetFd(),received_data.c_str(), result);
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
    File file(file_name);
    // Enviamos el tamaño
    std::string message_text(std::to_string(file.get_length()));
    message.size = sizeof(file.get_length());
    message_text.copy(message.text.data(), message.text.size() - 1, 0);
    emisor.send_to(message,remote_address);

    // Enviamos el nombre
    std::cout << "Nombre del nuevo archivo en el destino? : ";
    std::cin >> file_name;
    message_text = std::string(file_name);
    message.size = file_name.size();
    message_text.copy(message.text.data(), message.text.size() - 1, 0);
    emisor.send_to(message,remote_address);

    std::cout << "Enviando archivo: " << file_name << std::endl;

    int leidos = 0;

    bool fin = false;

    do{

      leidos = read(file.GetFd(), &message, sizeof(message.text));
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

int receive_mapped_file(std::string file_name, std::string ip_dest, int p_dest){

  sockaddr_in remote_address = make_ip_address(p_dest,ip_dest);
  Socket receptor(remote_address);
  
  //Obtenemos los metadatos del fichero
  sockaddr_in metaestructura{};
  std::string received_metadata;
  //Primero recibimos el tamaño
  
  receptor.receive_from(metaestructura, received_metadata);
  int sizef = stoi(received_metadata);
  //Luego el nombre
  receptor.receive_from(metaestructura, received_metadata);
  //fichero donde se guardara
  File file(received_metadata, sizef);
  std::cout << std::endl << "se creo el archivo nombre: " << file.get_name() 
                         << " y tamaño: " <<  file.get_length() << std::endl;

  sockaddr_in estructura{};
  std::string received_data;
  char buffer[SEND_CAPACITY];
  std::cout << std::endl << "Se leera desde el socket: " << receptor.get_fd() << std::endl;
  //reinterpret_cast<char*>(file.get_mmapped())
  //socklen_t size = sizeof(remote_address);

  int result = recvfrom(receptor.get_fd(), 
  reinterpret_cast<char*>(file.get_mmapped()), 
  SEND_CAPACITY, 0, 
  nullptr, nullptr);

  std::cout << std::endl << "Se leyo: " << buffer << std::endl;

  std::cout << std::endl << "FIN DEL ARCHIVO, HASTA LUEGO LUCA" << std::endl;

  return 0;
}

int send_mapped_file(std::string file_name, std::string ip_dest, int p_dest){

    std::string ip_address;
    int port = 0;
    //Direccion del emisor
    sockaddr_in direccion = make_ip_address(port,ip_address);
    Socket emisor(direccion);
    //Direccion del receptor
    sockaddr_in remote_address = make_ip_address(p_dest,ip_dest);
    
    Message message;
    File file(file_name);
    // Enviamos el tamaño
    std::string message_text(std::to_string(file.get_length()));
    message.size = sizeof(file.get_length());
    message_text.copy(message.text.data(), message.text.size() - 1, 0);
    emisor.send_to(message,remote_address);

    // Enviamos el nombre
    message_text = std::string("borrar.txt");
    message.size = file_name.size() - 2;
    message_text.copy(message.text.data(), message.text.size() - 1, 0);
    emisor.send_to(message,remote_address);

    // Empezamos a enviar el fichero
    //sleep(3);



    socklen_t size = sizeof(remote_address);

    sendto(emisor.get_fd(), 
    reinterpret_cast<char*>(file.get_mmapped()), 
    SEND_CAPACITY, 0, reinterpret_cast<const sockaddr*>(&remote_address), sizeof(remote_address));

    return 0;
}