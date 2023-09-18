#include <iostream>
#include "World Generator/Application.h"

int main() {
    WorldGenerator::Application app;

    try{
        app.Run();
    }catch(std::exception& e){
        std::cout << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
