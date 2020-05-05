#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <vector>
#include <cstring>
#include <optional>

const uint32_t window_width = 1024;
const uint32_t window_height = 768;

const std::vector<const char*> validationLayers{
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices{
    std::optional<uint32_t> graphicsFamily;
    bool isComplete(){
        return graphicsFamily.has_value();
    }
};

inline QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device){
    QueueFamilyIndices ret;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    int i = 0;
    for(const auto& queueFamily: queueFamilies){
        if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            ret.graphicsFamily = i;
        }
        ++ i;
    }
    return ret;
}

inline bool checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    bool allFound = true;
    for (const char *layerName : validationLayers)
    {
        bool found = false;
        for (const auto &layerProperty : availableLayers)
        {
            if (!strcmp(layerName, layerProperty.layerName))
            {
                found = true;
                break;
            }
        }
        allFound = (allFound && found);
        if (!allFound)
        {
            return false;
        }
    }

    return allFound;
}

inline bool isDeviceSuitable(VkPhysicalDevice device){
    QueueFamilyIndices indices = findQueueFamilies(device);
    return indices.isComplete();
}

inline int rateDevice(VkPhysicalDevice device){
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeats;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeats);

    if(!deviceFeats.geometryShader){
        return 0;
    }

    int ret = 0;
    ret += deviceProperties.deviceType * 1000;
    ret += deviceProperties.limits.maxImageDimension2D;
    return ret;
}
