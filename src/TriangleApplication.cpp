#include <TriangleApplication.hpp>
#include <stdexcept>
#include <vector>
#include <cstring>

/* private functions */

void TriangleApplication::initWindow(){
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(window_width, window_height, "vulkanTest", nullptr, nullptr);
}

void TriangleApplication::initVulkan(){
    createInstance();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
}

void TriangleApplication::createInstance(){
    if(enableValidationLayers){
        printf("* validation layer requested\n");
        if(!checkValidationLayerSupport()){
            throw std::runtime_error("** validation layer not available");
        }
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    // createInfo.enabledLayerCount = 0;
    if(enableValidationLayers){
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }
    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS){
        throw std::runtime_error("** failed to create instance");
    }

    uint32_t vkExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, nullptr);

    std::vector<VkExtensionProperties> vkExtensions(vkExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, vkExtensions.data());
    printf("* total supported extensions: [ %lu ]\n", vkExtensions.size());
    for(const auto& ext : vkExtensions){
        printf("* supported: %s\n", ext.extensionName);
    }
}

void TriangleApplication::createSurface(){
    if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS){
        throw std::runtime_error("** failed to create window surface");
    }
}

void TriangleApplication::pickPhysicalDevice(){
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if(deviceCount == 0){
        throw std::runtime_error("** failed to discover GPUs with vulkan");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    if(vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()) != VK_SUCCESS){
        throw std::runtime_error("** device discovered but failed to get devices");
    }
    // int maxScore = -1;
    // for(const auto& device : devices){
    //     int score = rateDevice(device);
    //     if(score > maxScore){
    //         maxScore = score;
    //         physicalDevice = device;
    //     }
    // }
    for(const auto& device : devices){
        if(isDeviceSuitable(device)){
            physicalDevice = device;
        }
    }
    if(physicalDevice == VK_NULL_HANDLE){
        throw std::runtime_error("** failed to find a suitable GPU");
    }
}

void TriangleApplication::createLogicalDevice(){
    QueueFamilyIndices indecies = findQueueFamilies(physicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indecies.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1.f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 0;
    if(enableValidationLayers){
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS){
        throw std::runtime_error("** failed to create logical device");
    }

    vkGetDeviceQueue(device, indecies.graphicsFamily.value(), 0, &graphicsQueue);
}

QueueFamilyIndices TriangleApplication::findQueueFamilies(VkPhysicalDevice device){
    QueueFamilyIndices ret;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    // int i = 0;
    // for(const auto& queueFamily: queueFamilies){
    for(int i = 0; i < queueFamilies.size() && !ret.isComplete(); ++ i){
        VkQueueFamilyProperties& queueFamily = queueFamilies[i];
        if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            ret.graphicsFamily = i;
        }
        
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if(presentSupport){
            ret.presentFamily = i;
        }
    }
    return ret;
}

bool TriangleApplication::isDeviceSuitable(VkPhysicalDevice device){
    QueueFamilyIndices indices = findQueueFamilies(device);
    return indices.isComplete();
}

void TriangleApplication::mainLoop(){
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }
}

void TriangleApplication::cleanup(){
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr); // destroy surface before destroying instance
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);
    glfwTerminate();
}

/* public function */

void TriangleApplication::run(){
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}
