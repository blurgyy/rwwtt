#include <iostream>
#include <exception>
#include <cstdlib>

#include <TriangleApplication.hpp>
#include <global.hpp>

int main(){
    TriangleApplication app;
    try{
        app.run();
    } catch(const std::exception& e){
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
