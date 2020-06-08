#pragma once

#include <pch.hpp>
#include <optional>
#include <vulkan/vulkan.h>
#include <vulkan/VulkanInstance.hpp>
#include "Window.hpp"

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

    static bool poSelected;

private:
    static cPhysicalDevice* poInstance;

    cWindow* ppWindow;

    VkPhysicalDevice poPhysicalDevice = VK_NULL_HANDLE;

    VkSampleCountFlagBits puiMaxSampleCount;

public:
    static cPhysicalDevice* GetInstance();

    void SelectPhysicalDevice(cVulkanInstance* pVulkanInstance, cWindow* pWindow);

    tQueueFamilyIndices FindQueueFamilies(void);
    tSwapChainSupportDetails QuerySwapChainSupport(void);
    VkSampleCountFlagBits GetMaxSampleCount(void);
    float GetMaxAnisotropy(void);

    bool CreateLogicalDevice(VkDeviceCreateInfo* pCreateInfo,
                             VkAllocationCallbacks* pAllocator,
                             VkDevice* pDevice);

    void GetPhysicalMemoryProperties(VkPhysicalDeviceMemoryProperties* pMemoryProperties);
    void GetDeviceFormatProperties(const VkFormat& oFormat,
                                   VkFormatProperties* pFormatProperties);

    void GetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice& oPhysicalDevice,
                                            uint uiQueueFamilyIndex,
                                            cWindow* pWindow,
                                            VkBool32* pSupported);

private:
    cPhysicalDevice();

    bool IsDeviceSuitable(VkPhysicalDevice& oDevice);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice& oDevice);

    tQueueFamilyIndices FindQueueFamilies(VkPhysicalDevice& oDevice);
    tSwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice& oDevice);
    VkSampleCountFlagBits GetMaxSampleCount(VkPhysicalDevice& oDevice);
    float GetMaxAnisotropy(VkPhysicalDevice& oDevice);
};

cPhysicalDevice* cPhysicalDevice::poInstance = new cPhysicalDevice();
bool cPhysicalDevice::poSelected = false;

cPhysicalDevice* cPhysicalDevice::GetInstance()
{
    return poInstance;
}

cPhysicalDevice::cPhysicalDevice()
{
}

void cPhysicalDevice::SelectPhysicalDevice(cVulkanInstance* pVulkanInstance, cWindow* pWindow)
{
    assert(pVulkanInstance != nullptr);
    assert(pWindow != nullptr);

    ppWindow = pWindow;

    // Get the number of graphics devices with Vulkan support
    uint uiDeviceCount = 0;
    pVulkanInstance->EnumeratePhysicalDevices(&uiDeviceCount, nullptr);

    // If none found, throw an error
    if (uiDeviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    // Get all the devices with Vulkan support
    std::vector<VkPhysicalDevice> aoDevices(uiDeviceCount);
    pVulkanInstance->EnumeratePhysicalDevices(&uiDeviceCount, aoDevices.data());

    // Set physicalDevice to the first device that is suitable
    for (VkPhysicalDevice& oDevice : aoDevices)
    {
        if (IsDeviceSuitable(oDevice))
        {
            poPhysicalDevice = oDevice;
            puiMaxSampleCount = GetMaxSampleCount(oDevice);
            break;
        }
    }

    // If no suitable device is found, throw an error
    if (poPhysicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    poSelected = true;
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
    return indices.IsComplete() && bExtensionsSupported && swapChainAdequate && tDeviceFeatures.samplerAnisotropy;
}

tQueueFamilyIndices cPhysicalDevice::FindQueueFamilies(VkPhysicalDevice& oDevice)
{
    // Get the amount of supported QueueFamilies
    uint ulQueueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(oDevice, &ulQueueFamilyCount, nullptr);

    // Get all the supported QueueFamilies
    std::vector<VkQueueFamilyProperties> atQueueFamilies(ulQueueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(oDevice, &ulQueueFamilyCount, atQueueFamilies.data());

    tQueueFamilyIndices tIndices;

    // Find one family that supports VK_QUEUE_GRAPHICS_BIT and store it in tIndices
    int iIndex = 0;
    bool bPresentFound = false;
    bool bGraphicsFound = false;
    for (const auto& tQueueFamily : atQueueFamilies)
    {
        VkBool32 presentSupport = false;
        GetPhysicalDeviceSurfaceSupportKHR(oDevice, iIndex, ppWindow, &presentSupport);
        if (presentSupport)
        {
            tIndices.oulPresentFamily = iIndex;
            bPresentFound = true;
        }

        if (tQueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            tIndices.oulGraphicsFamily = iIndex;
            bGraphicsFound = true;
        }

        if (bPresentFound && bGraphicsFound) break;

        iIndex++;
    }

    return tIndices;
}

bool cPhysicalDevice::CheckDeviceExtensionSupport(VkPhysicalDevice& oDevice)
{
    // Get the amount of supported extensions
    uint uiExtensionCount;
    vkEnumerateDeviceExtensionProperties(oDevice, nullptr, &uiExtensionCount, nullptr);

    // Get all the supported extensions
    std::vector<VkExtensionProperties> atAvailableExtensions(uiExtensionCount);
    vkEnumerateDeviceExtensionProperties(oDevice, nullptr, &uiExtensionCount, atAvailableExtensions.data());

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
    VkSurfaceKHR& oSurface = ppWindow->GetSurface();

    // Struct with information about swap chain support for this device
    tSwapChainSupportDetails tDetails = {};

    // Get the basic surface capabilities of this device (min/max number of images in swap chain, min/max width and height of images)
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(oDevice, oSurface, &tDetails.tCapabilities);

    // Get the surface formats supported by this device (pixel format, color space)
    uint uiFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(oDevice, oSurface, &uiFormatCount, nullptr);
    if (uiFormatCount != 0)
    {
        tDetails.atFormats.resize(uiFormatCount); //-V106
        vkGetPhysicalDeviceSurfaceFormatsKHR(oDevice, oSurface, &uiFormatCount, tDetails.atFormats.data());
    }

    // Get the presentation modes supported by this device
    uint presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(oDevice, oSurface, &presentModeCount, nullptr);
    if (presentModeCount != 0)
    {
        tDetails.atPresentModes.resize(presentModeCount); //-V106
        vkGetPhysicalDeviceSurfacePresentModesKHR(oDevice, oSurface, &presentModeCount, tDetails.atPresentModes.data());
    }

    return tDetails;
}

VkSampleCountFlagBits cPhysicalDevice::GetMaxSampleCount(VkPhysicalDevice& oDevice)
{
    VkPhysicalDeviceProperties tPhysicalDeviceProperties;
    vkGetPhysicalDeviceProperties(oDevice, &tPhysicalDeviceProperties);

    VkSampleCountFlags uiCounts = tPhysicalDeviceProperties.limits.framebufferColorSampleCounts &
                                  tPhysicalDeviceProperties.limits.framebufferDepthSampleCounts;

    if (uiCounts & VK_SAMPLE_COUNT_64_BIT) return VK_SAMPLE_COUNT_64_BIT;
    if (uiCounts & VK_SAMPLE_COUNT_32_BIT) return VK_SAMPLE_COUNT_32_BIT;
    if (uiCounts & VK_SAMPLE_COUNT_16_BIT) return VK_SAMPLE_COUNT_16_BIT;
    if (uiCounts & VK_SAMPLE_COUNT_8_BIT) return VK_SAMPLE_COUNT_8_BIT;
    if (uiCounts & VK_SAMPLE_COUNT_4_BIT) return VK_SAMPLE_COUNT_4_BIT;
    if (uiCounts & VK_SAMPLE_COUNT_2_BIT) return VK_SAMPLE_COUNT_2_BIT;

    return VK_SAMPLE_COUNT_1_BIT;
}

float cPhysicalDevice::GetMaxAnisotropy(VkPhysicalDevice& oDevice)
{
    VkPhysicalDeviceProperties tPhysicalDeviceProperties;
    vkGetPhysicalDeviceProperties(oDevice, &tPhysicalDeviceProperties);

    return tPhysicalDeviceProperties.limits.maxSamplerAnisotropy;
}

tQueueFamilyIndices cPhysicalDevice::FindQueueFamilies(void)
{
    return FindQueueFamilies(poPhysicalDevice);
}

tSwapChainSupportDetails cPhysicalDevice::QuerySwapChainSupport(void)
{
    return QuerySwapChainSupport(poPhysicalDevice);
}

VkSampleCountFlagBits cPhysicalDevice::GetMaxSampleCount(void)
{
    return puiMaxSampleCount;
}

float cPhysicalDevice::GetMaxAnisotropy(void)
{
    return GetMaxAnisotropy(poPhysicalDevice);
}

bool cPhysicalDevice::CreateLogicalDevice(VkDeviceCreateInfo* pCreateInfo,
                                          VkAllocationCallbacks* pAllocator,
                                          VkDevice* pLogicalDevice)
{
    assert(pCreateInfo != nullptr);
    assert(pLogicalDevice != nullptr);

    return vkCreateDevice(poPhysicalDevice, pCreateInfo, pAllocator, pLogicalDevice) == VK_SUCCESS;
}

void cPhysicalDevice::GetPhysicalMemoryProperties(VkPhysicalDeviceMemoryProperties* pMemoryProperties)
{
    assert(pMemoryProperties != nullptr);

    vkGetPhysicalDeviceMemoryProperties(poPhysicalDevice, pMemoryProperties);
}

void cPhysicalDevice::GetDeviceFormatProperties(const VkFormat& oFormat, VkFormatProperties* pFormatProperties) //-V669
{
    vkGetPhysicalDeviceFormatProperties(poPhysicalDevice, oFormat, pFormatProperties);
}

void cPhysicalDevice::GetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice& oPhysicalDevice,
                                                         uint uiQueueFamilyIndex,
                                                         cWindow* pWindow,
                                                         VkBool32* pSupported)
{
    vkGetPhysicalDeviceSurfaceSupportKHR(oPhysicalDevice, uiQueueFamilyIndex, pWindow->GetSurface(), pSupported);
}
