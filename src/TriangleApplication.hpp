#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

uint32_t window_width = 1024;
uint32_t window_height = 768;

class TriangleApplication
{
private:
    GLFWwindow *window;

    void initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(window_width, window_height, "vulkanTest", nullptr, nullptr);
    }
    void initVulkan() {}
    void mainLoop()
    {
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
        }
    }
    void cleanup()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

public:
    void run()
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }
};
