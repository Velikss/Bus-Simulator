#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/PhysicalDevice.hpp>

class cBufferHelper
{
public:
    static void CreateBuffer(cLogicalDevice* pLogicalDevice,
                             VkDeviceSize ulSize,
                             VkBufferUsageFlags uiUsage,
                             VkMemoryPropertyFlags uiProperties,
                             VkBuffer& oBuffer,
                             VkDeviceMemory& oBufferMemory);

    static uint FindMemoryType(uint uiTypeFilter, VkMemoryPropertyFlags tProperties);
};

void cBufferHelper::CreateBuffer(cLogicalDevice* pLogicalDevice,
                                 VkDeviceSize ulSize,
                                 VkBufferUsageFlags uiUsage,
                                 VkMemoryPropertyFlags uiProperties,
                                 VkBuffer& oBuffer,
                                 VkDeviceMemory& oBufferMemory)
{
    // Struct with information about the buffer
    VkBufferCreateInfo tBufferInfo = {};
    tBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    // Set the buffer size and usage
    tBufferInfo.size = ulSize;
    tBufferInfo.usage = uiUsage;

    // Buffers can be shared between queue families, we're not using this, so set to EXCLUSIVE
    tBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    // Create the vertex buffer
    if (!pLogicalDevice->CreateBuffer(&tBufferInfo, nullptr, &oBuffer))
    {
        throw std::runtime_error("failed to create buffer!");
    }

    // Struct with memory requirements for the vertex buffer
    VkMemoryRequirements tMemRequirements;
    vkGetBufferMemoryRequirements(pLogicalDevice->GetDevice(), oBuffer, &tMemRequirements);

    // Struct with information on how we want to allocate the buffer
    VkMemoryAllocateInfo tAllocInfo = {};
    tAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    // Set the required memory size
    tAllocInfo.allocationSize = tMemRequirements.size;

    // Find the right memory type and set it
    tAllocInfo.memoryTypeIndex = FindMemoryType(tMemRequirements.memoryTypeBits, uiProperties);

    // Allocate the memory for the buffer
    if (!pLogicalDevice->AllocateMemory(&tAllocInfo, nullptr, &oBufferMemory))
    {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    // Bind the allocated memory to the buffer
    pLogicalDevice->BindBufferMemory(oBuffer, oBufferMemory, 0);
}

uint cBufferHelper::FindMemoryType(uint uiTypeFilter, VkMemoryPropertyFlags tProperties)
{
    // Get memory properties from the physical device
    VkPhysicalDeviceMemoryProperties memProperties;
    cPhysicalDevice::GetInstance()->GetPhysicalMemoryProperties(&memProperties);

    // Find a memory type that matches the type filter and properties
    for (uint i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((uiTypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & tProperties) == tProperties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}
