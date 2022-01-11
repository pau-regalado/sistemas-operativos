#include "../include/funciones.h"

// Variables para el manejo de hilos y funcionalidades

bool ServerMode = false;
bool Abort = false, Resume = false, Pause = false, Quit = false;


int protected_main(int argc, char* argv[]) {

  std::exception_ptr eptr {};

  send_file(argv[1], "127.0.0.2", 5001);

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