#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

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

bool checkValidationLayerSupport()
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

int rateDevice(VkPhysicalDevice device){
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