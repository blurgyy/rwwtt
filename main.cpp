#include <vulkan/vulkan.h>
#include <iostream>
#include <exception>
#include <cstdlib>

class TriangleApplication
{
private:
    void initVulkan(){}
    void mainLoop(){}
    void cleanup(){}
public:
    void run(){
        initVulkan();
        mainLoop();
        cleanup();
    }
};

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
