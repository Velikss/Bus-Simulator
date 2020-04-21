#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/VulkanInstance.hpp>
#include "Surface.hpp"

// A struct for storing the QueueFamily indices
struct tQueueFamilyIndices
{
    std::optional<uint> oulGraphicsFamily;
    std::optional<uint> oulPresentFamily;

    // Return true if the oulGraphicsFamily optional has a value
    bool IsComplete()
    {
        return oulGraphicsFamily.has_value() && oulPresentFamily.has_value();
    }
};

struct tSwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR tCapabilities;
    std::vector<VkSurfaceFormatKHR> atFormats;
    std::vector<VkPresentModeKHR> atPresentModes;
};

class cPhysicalDevice
{
public:
    const std::vector<const char*> DEVICE_EXTENSIONS = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

private:
    cVulkanInstance* ppVulkanInstance;

    cSurface* ppSurface;

    // Selected physical graphics device
    VkPhysicalDevice poPhysicalDevice = VK_NULL_HANDLE;

public:
    cPhysicalDevice(cVulkanInstance* pVulkanInstance, cSurface* pSurface);

    tQueueFamilyIndices FindQueueFamilies(void);
    tSwapChainSupportDetails QuerySwapChainSupport(void);

    bool CreateLogicalDevice(VkDeviceCreateInfo* pCreateInfo,
                             VkAllocationCallbacks* pAllocator,
                             VkDevice* pDevice);

    void GetPhysicalMemoryProperties(VkPhysicalDeviceMemoryProperties* pMemoryProperties);

private:
    void SelectPhysicalDevice(void);

    bool IsDeviceSuitable(VkPhysicalDevice& oDevice);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice& oDevice);

    tQueueFamilyIndices FindQueueFamilies(VkPhysicalDevice& oDevice);
    tSwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice& oDevice);
};

cPhysicalDevice::cPhysicalDevice(cVulkanInstance* pVulkanInstance, cSurface* pSurface)
{
    assert(pVulkanInstance != NULL);
    assert(pSurface != NULL);

    ppVulkanInstance = pVulkanInstance;
    ppSurface = pSurface;

    SelectPhysicalDevice();
}

void cPhysicalDevice::SelectPhysicalDevice(void)
{
    // Get the number of graphics devices with Vulkan support
    uint deviceCount = 0;
    ppVulkanInstance->EnumeratePhysicalDevices(&deviceCount, NULL);

    // If none found, throw an error
    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    // Get all the devices with Vulkan support
    std::vector<VkPhysicalDevice> devices(deviceCount);
    ppVulkanInstance->EnumeratePhysicalDevices(&deviceCount, devices.data());

    // Set physicalDevice to the first device that is suitable
    for (VkPhysicalDevice& device : devices)
    {
        if (IsDeviceSuitable(device))
        {
            poPhysicalDevice = device;
            break;
        }
    }

    // If no suitable device is found, throw an error
    if (poPhysicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

bool cPhysicalDevice::IsDeviceSuitable(VkPhysicalDevice& oDevice)
{
    // Device properties include things like name, type and versions
    VkPhysicalDeviceProperties tDeviceProperties;
    vkGetPhysicalDeviceProperties(oDevice, &tDeviceProperties);
    // Device features include all the features supported by this device
    VkPhysicalDeviceFeatures tDeviceFeatures;
    vkGetPhysicalDeviceFeatures(oDevice, &tDeviceFeatures);

    tQueueFamilyIndices indices = FindQueueFamilies(oDevice);

    bool bExtensionsSupported = CheckDeviceExtensionSupport(oDevice);

    bool swapChainAdequate = false;
    if (bExtensionsSupported)
    {
        tSwapChainSupportDetails tSwapChainSupport = QuerySwapChainSupport(oDevice);
        swapChainAdequate = !tSwapChainSupport.atFormats.empty() && !tSwapChainSupport.atPresentModes.empty();
    }

    // Here we can check if the device is suitable and has the features we need
    // Right now we only care about a supported QueueFamily
    return indices.IsComplete() && bExtensionsSupported && swapChainAdequate;
}

tQueueFamilyIndices cPhysicalDevice::FindQueueFamilies(VkPhysicalDevice& oDevice)
{
    // Get the amount of supported QueueFamilies
    uint ulQueueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(oDevice, &ulQueueFamilyCount, NULL);

    // Get all the supported QueueFamilies
    std::vector<VkQueueFamilyProperties> atQueueFamilies(ulQueueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(oDevice, &ulQueueFamilyCount, atQueueFamilies.data());

    tQueueFamilyIndices tIndices;

    // Find one family that supports VK_QUEUE_GRAPHICS_BIT and store it in tIndices
    int iIndex = 0;
    for (const auto& tQueueFamily : atQueueFamilies)
    {
        VkBool32 presentSupport = false;
        ppSurface->GetPhysicalDeviceSurfaceSupportKHR(oDevice, iIndex, &presentSupport);
        if (presentSupport)
        {
            tIndices.oulPresentFamily = iIndex;
        }

        if (tQueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            tIndices.oulGraphicsFamily = iIndex;
        }
        iIndex++;
    }

    return tIndices;
}

bool cPhysicalDevice::CheckDeviceExtensionSupport(VkPhysicalDevice& oDevice)
{
    // Get the amount of supported extensions
    uint uiExtensionCount;
    vkEnumerateDeviceExtensionProperties(oDevice, NULL, &uiExtensionCount, NULL);

    // Get all the supported extensions
    std::vector<VkExtensionProperties> atAvailableExtensions(uiExtensionCount);
    vkEnumerateDeviceExtensionProperties(oDevice, NULL, &uiExtensionCount, atAvailableExtensions.data());

    // Create a set with the required extensions
    std::set<string> asRequiredExtensions(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());

    // Remove the supported extensions from the set
    for (VkExtensionProperties& tExtension : atAvailableExtensions)
    {
        asRequiredExtensions.erase(tExtension.extensionName);
    }

    // If the set is emtpy, all the required extensions are supported
    return asRequiredExtensions.empty();
}

tSwapChainSupportDetails cPhysicalDevice::QuerySwapChainSupport(VkPhysicalDevice& oDevice)
{
    // Get the VkSurfaceKHR object
    VkSurfaceKHR& oSurface = ppSurface->GetSurface();

    // Struct with information about swap chain support for this device
    tSwapChainSupportDetails tDetails = {};

    // Get the basic surface capabilities of this device (min/max number of images in swap chain, min/max width and height of images)
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(oDevice, oSurface, &tDetails.tCapabilities);

    // Get the surface formats supported by this device (pixel format, color space)
    uint uiFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(oDevice, oSurface, &uiFormatCount, NULL);
    if (uiFormatCount != 0)
    {
        tDetails.atFormats.resize(uiFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(oDevice, oSurface, &uiFormatCount, tDetails.atFormats.data());
    }

    // Get the presentation modes supported by this device
    uint presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(oDevice, oSurface, &presentModeCount, NULL);
    if (presentModeCount != 0)
    {
        tDetails.atPresentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(oDevice, oSurface, &presentModeCount, tDetails.atPresentModes.data());
    }

    return tDetails;
}

tQueueFamilyIndices cPhysicalDevice::FindQueueFamilies(void)
{
    return FindQueueFamilies(poPhysicalDevice);
}

tSwapChainSupportDetails cPhysicalDevice::QuerySwapChainSupport(void)
{
    return QuerySwapChainSupport(poPhysicalDevice);
}

bool cPhysicalDevice::CreateLogicalDevice(VkDeviceCreateInfo* pCreateInfo,
                                          VkAllocationCallbacks* pAllocator,
                                          VkDevice* pLogicalDevice)
{
    assert(pCreateInfo != NULL);
    assert(pLogicalDevice != NULL);

    return vkCreateDevice(poPhysicalDevice, pCreateInfo, pAllocator, pLogicalDevice) == VK_SUCCESS;
}

void cPhysicalDevice::GetPhysicalMemoryProperties(VkPhysicalDeviceMemoryProperties* pMemoryProperties)
{
    assert(pMemoryProperties != NULL);

    vkGetPhysicalDeviceMemoryProperties(poPhysicalDevice, pMemoryProperties);
}
