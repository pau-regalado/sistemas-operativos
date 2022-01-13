#include "../include/funciones.h"

// Variables para el manejo de hilos y funcionalidades

std::atomic<bool> ServerMode(true), Abort(false), 
                  Resume(false), Pause(false), Quit(false);
// Definiciones de tipos
typedef std::tuple<char,std::string,int> request_info;

// Peticiones de otros procesos por responder
std::vector<request_info> peticiones;

// Muestra todas las opciones y datos del servidor
void show_menu (void){
  std::cout << "-------------------------------------" << std::endl;
  std::cout << "Bienvenido a ViewNet!" << std::endl;
  std::cout << "Modo SERVIDOR " << ((ServerMode == true) ? "ON" : "OFF") << std::endl;
  std::cout << "Opciones:" << std::endl;
  std::cout << "[O / F] On / Off Server mode." << std::endl;
  std::cout << "[G]et fichero. Solicita el fichero." << std::endl;
  std::cout << "[L]ist. Lista los ficheros visibles." << std::endl;
  std::cout << "[A]bort. Aborta la recepcion de datos." << std::endl;
  std::cout << "[P]ause. Pausa el envio de datos." << std::endl;
  std::cout << "[R]esume. Reanuda el envio de datos." << std::endl;
  std::cout << "[Q]uit. Finaliza el programa." << std::endl;
  std::cout << "-------------------------------------" << std::endl;
}

void read_directory(const std::string& name, std::vector<std::string>& v){
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        v.push_back(dp->d_name);
    }
    closedir(dirp);
}

void list_dir(const std::string& name, std::string& res){
  
  std::vector<std::string> result;
  std::string resultado("|\n");
  read_directory(name.c_str(), result);
  for (int i = 0; i < result.size(); i++){
    if (result[i] != "." && result[i] != ".."){
      resultado += ("|- " + result[i] + "\n");
    }
  }
  res = resultado;
  //std::cout << "ficheros de " << name << ":" << std::endl;
  //std::cout << resultado << std::endl;
}

int receive_list(Socket& local, sockaddr_in& address, std::string& respuesta){
  std::cout << "Estoy esperando por que me manden public" << std::endl; 
  int a;
  try{
    a = local.receive_from(address, respuesta);
  }catch(std::system_error& e) {
    
    if (errno == EINTR){
      std::cout << "Proceso detenido" << std::endl;
    }else{
      std::cout << "Error procesando parametros" << std::endl;
    }
    return 99;
  }
  std::cout << "lei " << a << " bytes" << std::endl;
  std::cout << "El contenido del directorio public es:" << std::endl 
            << respuesta << std::endl;

  return 0;
}

int peticion_handler(std::exception_ptr& eptr, Socket& local){

  sockaddr_in request_sender_info{};
  std::string data_received;
  while (!Abort && ServerMode){
    std::cout << "Estoy esperando una solicitud" << std::endl;
    try{
      local.receive_from(request_sender_info, data_received);
    }catch(std::system_error& e) {
    
    if (errno == EINTR){
      std::cout << "Proceso detenido" << std::endl;
    }else{
      std::cout << "Error procesando parametros" << std::endl;
    }
    return 99;
  }

    char* remote_ip = inet_ntoa(request_sender_info.sin_addr);

    // Recuperamos el puerto del remitente en el orden adecuado para nuestra CPU

    int remote_port = ntohs(request_sender_info.sin_port);
    char peticion = data_received[0];
    peticiones.push_back(request_info(peticion,
                                      std::string(remote_ip),
                                      remote_port));

    sockaddr_in remote_address =  make_ip_address(remote_port,remote_ip);
    std::string directory;
    std::string public_files;
    Message list;
    std::cout << "he recibido: " << data_received << std::endl;
    switch (peticion){
    case 'L':
        
        directory = std::string("./public");
        list_dir(directory, public_files);

        public_files.copy(list.text.data(), list.text.size() - 1, 0);
        list.size = public_files.size();
        sleep(3);
        local.send_to(list, remote_address);
        std::cout << "Contenido de ./public enviado con exito." << std::endl;
      break;

    case 'G':{
        std::string filename = data_received.substr(1,data_received.size()-1);
        sleep(3);
        send_file(filename,remote_ip, remote_port);
        std::cout << "Fichero " << filename << " enviado con exito." << std::endl;
      break;
    }
      default:
        std::cout << "BASURA: " <<  data_received << std::endl;
    }
  }

  return 0;
}

int commands_processor(std::exception_ptr& eptr, Socket& local){
  try{
    char command;

    std::thread petition_hadler_thread(&peticion_handler, std::ref(eptr), std::ref(local));
    

    // Variables para Opcion LIST
    std::thread send_list;
    std::thread hilo3;
    std::string directory;
    std::string public_files;
    std::string l_op("L");
    std::string g_op("G");

    while(!Quit){
      show_menu();
      std::cin >> command;
      switch (command){
        case 'Q':
          Abort = true;
          Quit = true;
          pthread_kill(petition_hadler_thread.native_handle(), SIGUSR1);
          pthread_kill(hilo3.native_handle(), SIGUSR1);
        break;
        
        case 'O':
          ServerMode = true;
        break;

        case 'F':
          ServerMode = false;
        break;

        case 'A':
          Abort = true;
        break;
        
        case 'L':{

          Message opcion;
          sockaddr_in datos_remitente;
          std::string ip_public("127.0.0.5");
          std::string contenido_public;
          int port_public = 5009;
          Socket receptor_public(make_ip_address(port_public, ip_public));

          // Preparamos el mensaje
          l_op.copy(opcion.text.data(), opcion.text.size() - 1, 0);
          opcion.size = 1;

          // Si no hay ninguna peticion
          if(peticiones.size() > 0){
            std::cout << "voy a enviar la L a IP: " << std::get<1>(peticiones[0]) << " puerto: " << std::get<2>(peticiones[0]) << std::endl;
            receptor_public.send_to(opcion, std::get<1>(peticiones[0]), std::get<2>(peticiones[0]));
            hilo3 = std::thread(&receive_list, std::ref(receptor_public), std::ref(datos_remitente), std::ref(contenido_public));
            hilo3.join();
            std::cout << "Envie la L" << std::endl;
          }else{
            std::cout << "voy a enviar la L a IP: " << "127.0.0.2" << " puerto: " << 5001 << std::endl;
            receptor_public.send_to(opcion, "127.0.0.2", 5001);
            std::cout << "Envie la L" << std::endl;
            hilo3 = std::thread(&receive_list, std::ref(receptor_public), std::ref(datos_remitente), std::ref(contenido_public));
            hilo3.join();
          }
        break;
        }

        case 'G':{
          std::string filename;
          std::cout << "Introduzca el nombre del archivo deseado: ";
          std::cin >> filename;
          Message opcion;
          sockaddr_in datos_remitente;
          std::string ip_get_file("127.0.0.6");
          std::string contenido_public;
          int port_get_file = 5010;
          Socket* send_request = new Socket(make_ip_address(port_get_file, ip_get_file));

          // Preparamos el mensaje
          g_op = g_op + filename;
          g_op.copy(opcion.text.data(), opcion.text.size() - 1, 0);
          opcion.size = g_op.size();

          // Si no hay ninguna peticion
          if(peticiones.size() > 0){
            std::cout << "voy a enviar la G a IP: " << std::get<1>(peticiones[0]) << " puerto: " << std::get<2>(peticiones[0]) << std::endl;
            send_request->send_to(opcion, std::get<1>(peticiones[0]), std::get<2>(peticiones[0]));
            delete send_request;
            hilo3 = std::thread(&receive_file, std::ref(filename), std::ref(ip_get_file), std::ref(port_get_file));
            hilo3.join();
            std::cout << "Envie la G" << std::endl;
          }else{
            std::cout << "voy a enviar la G a IP: " << "127.0.0.2" << " puerto: " << 5001 << std::endl;
            send_request->send_to(opcion, "127.0.0.2", 5001);
            delete send_request;
            std::cout << "Envie la G" << std::endl;
            hilo3 = std::thread(&receive_file, std::ref(filename), std::ref(ip_get_file), std::ref(port_get_file));
            hilo3.join();
          }
        break;
        }
      } 
    }
    petition_hadler_thread.join();
    std::cout << "adios commands" << std::endl;
  }catch(std::exception& e) {
    std::cout << "Error procesando parametros" << std::endl;
    eptr = std::current_exception();
    return 99;
  }
}

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
  File file(file_name, sizef);

  std::string received_data;
  sockaddr_in estructura{};
  int result;

  do{
    result = receptor.receive_from(estructura, received_data);
    //std::cout << "Se han leido " << result << " bytes" << std::endl;
    if (result > 0)
      write(file.GetFd(),received_data.c_str(), result);
  }while(result > 0);

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
    File file("./public/" + file_name);
    // Enviamos el tamaño
    std::string message_text(std::to_string(file.get_length()));
    message.size = sizeof(file.get_length());
    message_text.copy(message.text.data(), message.text.size() - 1, 0);
    emisor.send_to(message,remote_address);

    // Enviamos el nombre
    //std::cout << "Nombre del nuevo archivo en el destino? : ";
    //std::cin >> file_name;
    message_text = std::string(file_name);
    message.size = file_name.size();
    message_text.copy(message.text.data(), message.text.size() - 1, 0);
    emisor.send_to(message,remote_address);

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