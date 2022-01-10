#include "../include/funciones.h"

int protected_main(int argc, char* argv[]) {
    
    receive_file(argv[1], "127.0.0.2", 5001);

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