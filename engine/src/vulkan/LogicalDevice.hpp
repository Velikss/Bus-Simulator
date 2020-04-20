#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/PhysicalDevice.hpp>

class cLogicalDevice
{
private:
    // Logical device handle
    VkDevice poDevice;

    // Supported QueueFamilies
    tQueueFamilyIndices ptQueueIndices;

    // Handle for interfacing with the graphics queue
    VkQueue poGraphicsQueue;

    // Handle for interfacing with the presentation queue
    VkQueue poPresentQueue;

public:
    cLogicalDevice(cPhysicalDevice* pPhysicalDevice);
    ~cLogicalDevice(void);

    VkDevice& GetDevice(void);

private:
    VkDeviceQueueCreateInfo GetQueueCreateInfo(uint uiQueueFamily);
    VkDeviceCreateInfo GetDeviceCreateInfo(std::vector<VkDeviceQueueCreateInfo>& atQueueCreateInfos,
                                           VkPhysicalDeviceFeatures& tDeviceFeatures,
                                           const std::vector<const char*>& apDeviceExtensions);
};

cLogicalDevice::cLogicalDevice(cPhysicalDevice* pPhysicalDevice)
{
    assert(pPhysicalDevice != NULL);

    // Get supported QueueFamilies from the physical device
    ptQueueIndices = pPhysicalDevice->FindQueueFamilies();

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    uint uniqueQueueFamilies[] = {
            ptQueueIndices.oulGraphicsFamily.value(),
            ptQueueIndices.oulPresentFamily.value()
    };

    // Struct with information about the physical device features we want to enable
    VkPhysicalDeviceFeatures tDeviceFeatures = {};

    for (uint uiQueueFamily : uniqueQueueFamilies)
    {
        queueCreateInfos.push_back(GetQueueCreateInfo(uiQueueFamily));
    }

    VkDeviceCreateInfo tDeviceCreateInfo = GetDeviceCreateInfo(queueCreateInfos, tDeviceFeatures,
                                                               pPhysicalDevice->DEVICE_EXTENSIONS);

    // Create the logical device, and throw an error on failure
    if (!pPhysicalDevice->CreateLogicalDevice(&tDeviceCreateInfo, NULL, &poDevice))
    {
        throw std::runtime_error("failed to create logical device!");
    }

    // Get the graphics queue handle from the logical device
    vkGetDeviceQueue(poDevice, ptQueueIndices.oulGraphicsFamily.value(), 0, &poGraphicsQueue);

    // Get the presentation queue handle from the logical device
    vkGetDeviceQueue(poDevice, ptQueueIndices.oulPresentFamily.value(), 0, &poPresentQueue);
}

cLogicalDevice::~cLogicalDevice()
{
    // Destroy the logical device
    vkDestroyDevice(poDevice, NULL);
}

VkDeviceQueueCreateInfo cLogicalDevice::GetQueueCreateInfo(uint uiQueueFamily)
{
    // Struct with information about for creating the Queue
    VkDeviceQueueCreateInfo tQueueCreateInfo = {};
    tQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    tQueueCreateInfo.queueCount = 1;

    // Set the family index
    tQueueCreateInfo.queueFamilyIndex = uiQueueFamily;

    // Priority is required even with only a single queue
    float fQueuePriority = 1.0f;
    tQueueCreateInfo.pQueuePriorities = &fQueuePriority;

    return tQueueCreateInfo;
}

VkDeviceCreateInfo cLogicalDevice::GetDeviceCreateInfo(std::vector<VkDeviceQueueCreateInfo>& atQueueCreateInfos,
                                                       VkPhysicalDeviceFeatures& tDeviceFeatures,
                                                       const std::vector<const char*>& apDeviceExtensions)
{
    // Struct with info for creating the logical device
    VkDeviceCreateInfo tCreateInfo = {};
    tCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    // Set the queue create info
    tCreateInfo.queueCreateInfoCount = static_cast<uint>(atQueueCreateInfos.size());
    tCreateInfo.pQueueCreateInfos = atQueueCreateInfos.data();

    // Set the device features we want to enable
    tCreateInfo.pEnabledFeatures = &tDeviceFeatures;

    // Set the device extensions we want to enable
    tCreateInfo.enabledExtensionCount = static_cast<uint>(apDeviceExtensions.size());
    tCreateInfo.ppEnabledExtensionNames = apDeviceExtensions.data();

    // Setup validation layers if they are enabled
    tCreateInfo.enabledLayerCount = 0;

    return tCreateInfo;
}

VkDevice& cLogicalDevice::GetDevice()
{
    return poDevice;
}
