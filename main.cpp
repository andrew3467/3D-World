#include <iostream>
#include "Engine/Application.h"

int main() {
    Engine::Application app;

    try{
        app.Run();
    }catch(std::exception& e){
        std::cout << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
