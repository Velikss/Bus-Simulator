#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

class cVulkanInstance
{
private:
    const std::vector<const char*> pasValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

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
    bool CreateWindowSurface(cWindow* pWindow,
                             VkAllocationCallbacks* pAllocatorCallback,
                             VkSurfaceKHR* pSurface);
    void DestroyWindowSurface(VkSurfaceKHR& oSurface,
                              VkAllocationCallbacks* pAllocatorCallback);

private:
    VkApplicationInfo GetApplicationInfo(void);
    VkInstanceCreateInfo GetCreateInfo(VkApplicationInfo& tAppInfo);

    bool CheckValidationLayerSupport();
};

cVulkanInstance::cVulkanInstance(void)
{
    if (pbENABLE_VALIDATION_LAYERS && !CheckValidationLayerSupport())
    {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo tAppInfo = GetApplicationInfo();
    VkInstanceCreateInfo tCreateInfo = GetCreateInfo(tAppInfo);

    if (pbENABLE_VALIDATION_LAYERS)
    {
        tCreateInfo.enabledLayerCount = pasValidationLayers.size();
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

VkApplicationInfo cVulkanInstance::GetApplicationInfo(void)
{
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

    return tAppInfo;
}

VkInstanceCreateInfo cVulkanInstance::GetCreateInfo(VkApplicationInfo& tAppInfo)
{
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

    return tCreateInfo;
}

void cVulkanInstance::EnumeratePhysicalDevices(uint* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices)
{
    vkEnumeratePhysicalDevices(poInstance, pPhysicalDeviceCount, pPhysicalDevices);
}

bool cVulkanInstance::CreateWindowSurface(cWindow* pWindow,
                                          VkAllocationCallbacks* pAllocatorCallback,
                                          VkSurfaceKHR* pSurface)
{
    return pWindow->CreateWindowSurface(poInstance, pAllocatorCallback, pSurface);
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
