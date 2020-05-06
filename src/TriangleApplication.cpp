#include <TriangleApplication.hpp>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <set>

/* private functions */

void TriangleApplication::initWindow(){
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(window_width, window_height, "vulkanTest", nullptr, nullptr);
}

void TriangleApplication::initVulkan(){
    createInstance();
    setupDebugMessenger();
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

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    if(enableValidationLayers){
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS){
        throw std::runtime_error("** failed to create instance");
    }

    // // output a list of supported extensions
    // uint32_t ExtensionCount = 0;
    // vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr);

    // std::vector<VkExtensionProperties> availableExtensions(ExtensionCount);
    // vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, availableExtensions.data());
    // printf("* total supported extensions: [ %lu ]\n", availableExtensions.size());
    // for(const auto& ext : availableExtensions){
    //     printf("* supported: %s\n", ext.extensionName);
    // }
}

void TriangleApplication::setupDebugMessenger(){
    if(!enableValidationLayers){
        return;
    }
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);
    if(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS){
        throw std::runtime_error("** failed to set up debug messenger");
    }
}
void TriangleApplication::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo){
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional
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
    printf("found %d devices\n", static_cast<int>(devices.size()));
    for(const auto& device : devices){
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        printf("Device name: %s\n", deviceProperties.deviceName);
        if(isDeviceSuitable(device)){
            physicalDevice = device;
            // break;
        }
    }
    if(physicalDevice == VK_NULL_HANDLE){
        throw std::runtime_error("** failed to find a suitable GPU");
    }
}

void TriangleApplication::createLogicalDevice(){
    QueueFamilyIndices indecies = findQueueFamilies(physicalDevice);

    // VkDeviceQueueCreateInfo queueCreateInfo{};
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = indecies.getUniqueSet();
    float queuePriority = 1.f;
    for(const uint32_t& queueFamily : uniqueQueueFamilies){
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pEnabledFeatures = &deviceFeatures;

    // createInfo.enabledExtensionCount = 0;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    
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
    vkGetDeviceQueue(device, indecies.presentFamily.value(), 0, &presentQueue);
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

SwapChainSupportDetails TriangleApplication::querySwapChainSupport(VkPhysicalDevice device){
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if(formatCount != 0){
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if(presentModeCount != 0){
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

bool TriangleApplication::isDeviceSuitable(VkPhysicalDevice device){
    QueueFamilyIndices indices = findQueueFamilies(device);
    // bool extensionSupported = checkDeviceExtensionSupport(device);
    bool extensionSupported = true;
    bool swapChainAdequate = false;
    if(extensionSupported){
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
    // printf("ext support: %s\n", extensionSupported ? "yes" : "no");
    return indices.isComplete() && extensionSupported && swapChainAdequate;
}

bool TriangleApplication::checkDeviceExtensionSupport(VkPhysicalDevice device){
    uint32_t ExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(ExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for(const auto& ext : availableExtensions){
        requiredExtensions.erase(ext.extensionName);
    }

    return requiredExtensions.empty();
}

std::vector<const char*> TriangleApplication::getRequiredExtensions(){
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    if(enableValidationLayers){
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return extensions;
}

void TriangleApplication::mainLoop(){
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }
}

void TriangleApplication::cleanup(){
    if(enableValidationLayers){
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
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
