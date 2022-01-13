#include "../include/funciones.h"

void usr1_handler(int signal) {

  std::cout << std::endl << "Cerraando..." << std::endl;

}
void set_signals(void){
  sigset_t sigwaitset;
  sigemptyset( &sigwaitset );
  sigaddset  ( &sigwaitset, SIGALRM);
  sigaddset  ( &sigwaitset, SIGTERM);
  sigaddset  ( &sigwaitset, SIGINT);

  int signal;

  sigwait( &sigwaitset, &signal);

  if (signal == SIGTERM | signal == SIGINT | signal == SIGHUP){
    //Quit = false;
    return;
  }
}

int protected_main(int argc, char* argv[]) {

  // Manejo de señales

  struct sigaction act{};

  act.sa_handler = &usr1_handler;
  int res = sigaction (SIGUSR1, &act, nullptr);

    //señales del programa

  //signal(SIGINT, &usr1_handler); //cuando se usa CTRL+C

  //signal(SIGTERM, &usr1_handler); //enviada antes de apagar el sistema

  //signal(SIGHUP, &usr1_handler); //cuando se cierra la ventana de la terminal

  std::thread signals_handler(&set_signals);

  std::exception_ptr eptr {};

  std::string ip_address;
  int port;

  if (argc == 1){
    ip_address = "127.0.0.2";
    port = 5001;
    std::cout << "Cree sin arg" << std::endl;
  }else{
    ip_address = argv[1];
    std::string aux(argv[2]);
    port = atoi(aux.c_str());
    std::cout << "Cree con arg: " << ip_address << " " << port << std::endl;
  }
  //Direccion del emisor
  sockaddr_in direccion = make_ip_address(port,ip_address);
  Socket Local(direccion);

  std::thread control_center(&commands_processor, std::ref(eptr), std::ref(Local));
  
  control_center.join(); 

  if (eptr){
    std::rethrow_exception(eptr);
  }

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