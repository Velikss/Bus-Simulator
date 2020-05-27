#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/PhysicalDevice.hpp>
#include <vulkan/command/CommandHelper.hpp>

// Helper class for dealing with Vulkan buffers and device memory
class cBufferHelper
{
public:
    // Sets up a new buffer on the device and copies the data to it
    static void CopyToNewBuffer(cLogicalDevice* pLogicalDevice,
                                VkMemoryPropertyFlags uiProperties,
                                void* pData, VkDeviceSize ulSize,
                                VkBuffer& oBuffer, VkDeviceMemory& oBufferMemory);

    // Creates a buffer based on specified parameters
    static void CreateBuffer(cLogicalDevice* pLogicalDevice,
                             VkDeviceSize ulSize,
                             VkBufferUsageFlags uiUsage,
                             VkMemoryPropertyFlags uiProperties,
                             VkBuffer& oBuffer,
                             VkDeviceMemory& oBufferMemory);

    // Copies the data in a buffer to another buffer
    static void CopyBuffer(VkBuffer& oSrcBuffer,
                           VkBuffer& oDstBuffer,
                           VkDeviceSize ulSize,
                           cLogicalDevice* pLogicalDevice);

    // Find a memory type which is matching the filter and flags and is
    // supported by the selected physical device
    static uint FindMemoryType(uint uiTypeFilter, VkMemoryPropertyFlags tProperties);
};

void cBufferHelper::CopyToNewBuffer(cLogicalDevice* pLogicalDevice,
                                    VkMemoryPropertyFlags uiProperties,
                                    void* pData, VkDeviceSize ulSize,
                                    VkBuffer& oBuffer, VkDeviceMemory& oBufferMemory)
{
    assert(pLogicalDevice != nullptr);  // can't create a buffer on a non-existent device
    assert(pData != nullptr);           // can't create an empty buffer
    assert(ulSize > 0);                 // can't create an empty buffer

    // Create a stagingBuffer which is host visible and coherent so we can copy to it
    VkBuffer oStagingBuffer;
    VkDeviceMemory oStagingBufferMemory;
    cBufferHelper::CreateBuffer(pLogicalDevice, ulSize,
                                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                oStagingBuffer, oStagingBufferMemory);

    // Pointer for mapped memory
    void* pBuffer;

    // Map the pBuffer pointer to the stagingBufferMemory
    pLogicalDevice->MapMemory(oStagingBufferMemory, 0, ulSize, 0, &pBuffer);
    {
        // Copy the data to the mapped memory
        memcpy(pBuffer, pData, (uint) ulSize); //-V106
    }
    // Unmap the memory again
    pLogicalDevice->UnmapMemory(oStagingBufferMemory);

    // Create a vertexBuffer which is local to the graphics device
    cBufferHelper::CreateBuffer(pLogicalDevice, ulSize,
                                VK_BUFFER_USAGE_TRANSFER_DST_BIT | uiProperties,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                oBuffer, oBufferMemory);

    // Copy the data from the stagingBuffer to the vertexBuffer
    CopyBuffer(oStagingBuffer, oBuffer, ulSize, pLogicalDevice);

    // Clean up the stagingBuffer since we don't need it anymore
    pLogicalDevice->DestroyBuffer(oStagingBuffer, nullptr);
    pLogicalDevice->FreeMemory(oStagingBufferMemory, nullptr);
}

void cBufferHelper::CreateBuffer(cLogicalDevice* pLogicalDevice,
                                 VkDeviceSize ulSize,
                                 VkBufferUsageFlags uiUsage,
                                 VkMemoryPropertyFlags uiProperties,
                                 VkBuffer& oBuffer,
                                 VkDeviceMemory& oBufferMemory)
{
    assert(pLogicalDevice != nullptr);  // can't create a buffer on a non-existent device
    assert(ulSize > 0);                 // can't create an empty buffer

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

void cBufferHelper::CopyBuffer(VkBuffer& oSrcBuffer,
                               VkBuffer& oDstBuffer,
                               VkDeviceSize ulSize,
                               cLogicalDevice* pLogicalDevice)
{
    assert(oSrcBuffer != VK_NULL_HANDLE);   // source buffer must exist
    assert(oDstBuffer != VK_NULL_HANDLE);   // destination buffer must exist
    assert(pLogicalDevice != nullptr);      // can't create a buffer on a non-existent device
    assert(ulSize > 0);                     // can't copy an empty buffer

    // Struct with information about the copy operation
    VkBufferCopy tCopyRegion = {};
    tCopyRegion.srcOffset = 0;
    tCopyRegion.dstOffset = 0;
    tCopyRegion.size = ulSize;

    VkCommandBuffer oCommandBuffer = cCommandHelper::BeginSingleTimeCommands(pLogicalDevice);
    {
        // Copy Buffer operation
        vkCmdCopyBuffer(oCommandBuffer, oSrcBuffer, oDstBuffer, 1, &tCopyRegion);
    }
    cCommandHelper::EndSingleTimeCommands(pLogicalDevice, oCommandBuffer);
}

uint cBufferHelper::FindMemoryType(uint uiTypeFilter, VkMemoryPropertyFlags tProperties)
{
    assert(cPhysicalDevice::poSelected); // A physical device must be selected first

    // Get memory properties from the physical device
    VkPhysicalDeviceMemoryProperties tMemProperties;
    cPhysicalDevice::GetInstance()->GetPhysicalMemoryProperties(&tMemProperties);

    // Find a memory type that matches the type filter and properties
    for (uint i = 0; i < tMemProperties.memoryTypeCount; i++)
    {
        if ((uiTypeFilter & (1 << i)) && (tMemProperties.memoryTypes[i].propertyFlags & tProperties) == tProperties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}
