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
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice;
    VkDevice device;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    void initWindow();
    void initVulkan();

    void createInstance();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSurface();

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    bool isDeviceSuitable(VkPhysicalDevice device);
    void mainLoop();
    void cleanup();

public:
    void run();
};
