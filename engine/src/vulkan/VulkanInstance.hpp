#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

class cVulkanInstance
{
private:
    // List of validation layers we want to enable
    // Validation layers allow you to see warnings and errors from Vulkan
    const std::vector<const char*> pasValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

    // Validation layers should be disabled for production builds
    // as they have a significant performance impact
#ifdef NDEBUG
    const bool pbENABLE_VALIDATION_LAYERS = false;
#else
    const bool pbENABLE_VALIDATION_LAYERS = true;
#endif

    VkInstance poInstance;

public:
    cVulkanInstance(void);
    ~cVulkanInstance(void);

    void EnumeratePhysicalDevices(uint* pPhysicalDeviceCount,
                                  VkPhysicalDevice* pPhysicalDevices);
    bool CreateWindowSurface(GLFWwindow* pWindow,
                             VkAllocationCallbacks* pAllocatorCallback,
                             VkSurfaceKHR* pSurface);
    void DestroyWindowSurface(VkSurfaceKHR& oSurface,
                              VkAllocationCallbacks* pAllocatorCallback);

private:
    bool CheckValidationLayerSupport();
};

cVulkanInstance::cVulkanInstance(void)
{
    // If validation layers are enabled, check to make sure all the
    // requested layers are supported
    if (pbENABLE_VALIDATION_LAYERS && !CheckValidationLayerSupport())
    {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    // Struct with information about our application
    VkApplicationInfo tAppInfo = {};
    tAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

    // Name + version of our application
    tAppInfo.pApplicationName = "Vulkan Engine";
    tAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

    // Can be used to tell Vulkan what engine we're using
    tAppInfo.pEngineName = "No Engine";
    tAppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    // Vulkan API version
    tAppInfo.apiVersion = VK_API_VERSION_1_0;

    // Struct with information for creating the Vulkan instance
    VkInstanceCreateInfo tCreateInfo = {};
    tCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    tCreateInfo.pApplicationInfo = &tAppInfo;

    // Get the extensions required for GLFW
    uint ulGLFWExtensionCount = 0;
    const char** ppGLFWExtensions = glfwGetRequiredInstanceExtensions(&ulGLFWExtensionCount);

    tCreateInfo.enabledExtensionCount = ulGLFWExtensionCount;
    tCreateInfo.ppEnabledExtensionNames = ppGLFWExtensions;

    // If validation layers are disabled, set the count to 0
    tCreateInfo.enabledLayerCount = 0;

    // If validation layers are enabled, add them to the create info
    if (pbENABLE_VALIDATION_LAYERS)
    {
        tCreateInfo.enabledLayerCount = (uint)pasValidationLayers.size();
        tCreateInfo.ppEnabledLayerNames = pasValidationLayers.data();
    }
    else
    {
        tCreateInfo.enabledLayerCount = 0;
    }

    // Create the Vulkan instance, and throw an error on failure
    if (vkCreateInstance(&tCreateInfo, nullptr, &poInstance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }
}

cVulkanInstance::~cVulkanInstance(void)
{
    // Destroy the Vulkan instance
    vkDestroyInstance(poInstance, nullptr);
}

void cVulkanInstance::EnumeratePhysicalDevices(uint* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices)
{
    vkEnumeratePhysicalDevices(poInstance, pPhysicalDeviceCount, pPhysicalDevices);
}

bool cVulkanInstance::CreateWindowSurface(GLFWwindow* pWindow,
                                          VkAllocationCallbacks* pAllocatorCallback,
                                          VkSurfaceKHR* pSurface)
{
    return glfwCreateWindowSurface(poInstance, pWindow, pAllocatorCallback, pSurface) == VK_SUCCESS;
}

void cVulkanInstance::DestroyWindowSurface(VkSurfaceKHR& oSurface, VkAllocationCallbacks* pAllocatorCallback)
{
    vkDestroySurfaceKHR(poInstance, oSurface, pAllocatorCallback);
}

bool cVulkanInstance::CheckValidationLayerSupport()
{
    uint uiLayerCount;
    vkEnumerateInstanceLayerProperties(&uiLayerCount, nullptr);

    std::vector<VkLayerProperties> atAvailableLayers(uiLayerCount);
    vkEnumerateInstanceLayerProperties(&uiLayerCount, atAvailableLayers.data());

    for (const char* sLayerName : pasValidationLayers)
    {
        bool bLayerFound = false;

        for (VkLayerProperties& tLayerProperties : atAvailableLayers)
        {
            if (strcmp(sLayerName, tLayerProperties.layerName) == 0)
            {
                bLayerFound = true;
                break;
            }
        }

        if (!bLayerFound)
        {
            return false;
        }
    }

    return true;
}
