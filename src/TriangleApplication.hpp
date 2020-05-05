#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

class TriangleApplication
{
private:
    GLFWwindow *window;

    void initWindow();
    void initVulkan() {}
    void mainLoop();
    void cleanup();

public:
    void run();
};
