#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <global.hpp>

class TriangleApplication
{
private:
    GLFWwindow *window;
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkSurfaceKHR surface;

    void initWindow();
    void initVulkan();

    void createInstance();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSurface();

    void mainLoop();
    void cleanup();

public:
    void run();
};
