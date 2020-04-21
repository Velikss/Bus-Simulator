#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/Vertex.hpp>
#include <vulkan/LogicalDevice.hpp>

class cVertexBuffer
{
private:
    const std::vector<Vertex> patVERTICES = {
            {{0.0f,  -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f},  {0.0f, 0.0f, 1.0f}}
    };

    cLogicalDevice* ppLogicalDevice;

    VkBuffer poVertexBuffer;
    VkDeviceMemory poVertexBufferMemory;

public:
    cVertexBuffer(cLogicalDevice* pLogicalDevice,
                  cPhysicalDevice* pPhysicalDevice,
                  VkCommandPool& oCommandPool);
    ~cVertexBuffer(void);

    uint GetVertexCount(void);

    void CmdBindVertexBuffer(VkCommandBuffer& oCommandBuffer);

private:
    void CreateBuffer(cLogicalDevice* pLogicalDevice,
                      cPhysicalDevice* pPhysicalDevice,
                      VkDeviceSize ulSize,
                      VkBufferUsageFlags uiUsage,
                      VkMemoryPropertyFlags uiProperties,
                      VkBuffer& oBuffer,
                      VkDeviceMemory& oBufferMemory);
    void CopyBuffer(cLogicalDevice* pLogicalDevice,
                    VkBuffer& oSrcBuffer,
                    VkBuffer& oDstBuffer,
                    VkDeviceSize ulSize,
                    VkCommandPool& oCommandPool);

    uint FindMemoryType(uint uiTypeFilter, VkMemoryPropertyFlags tProperties, cPhysicalDevice* pPhysicalDevice);
};

cVertexBuffer::cVertexBuffer(cLogicalDevice* pLogicalDevice,
                             cPhysicalDevice* pPhysicalDevice,
                             VkCommandPool& oCommandPool)
{
    ppLogicalDevice = pLogicalDevice;

    VkDeviceSize ulBufferSize = sizeof(patVERTICES[0]) * patVERTICES.size();

    // Create a stagingBuffer which is host visible and coherent so we can copy to it
    VkBuffer oStagingBuffer;
    VkDeviceMemory oStagingBufferMemory;
    CreateBuffer(pLogicalDevice, pPhysicalDevice, ulBufferSize,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 oStagingBuffer, oStagingBufferMemory);

    // Pointer for mapped memory
    void* pData;

    // Map the memory into CPU accessible memory
    pLogicalDevice->MapMemory(oStagingBufferMemory, 0, ulBufferSize, 0, &pData);
    {
        // Copy the vertex data to the mapped memory
        memcpy(pData, patVERTICES.data(), (uint) ulBufferSize);
    }
    // Unmap the memory again
    pLogicalDevice->UnmapMemory(oStagingBufferMemory);

    // Create a vertexBuffer which is local to the graphics device
    CreateBuffer(pLogicalDevice, pPhysicalDevice, ulBufferSize,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 poVertexBuffer, poVertexBufferMemory);

    // Copy the data from the stagingBuffer to the vertexBuffer
    CopyBuffer(pLogicalDevice, oStagingBuffer, poVertexBuffer, ulBufferSize, oCommandPool);

    // Clean up the stagingBuffer
    pLogicalDevice->DestroyBuffer(oStagingBuffer, NULL);
    pLogicalDevice->FreeMemory(oStagingBufferMemory, NULL);
}

cVertexBuffer::~cVertexBuffer(void)
{
    ppLogicalDevice->DestroyBuffer(poVertexBuffer, NULL);
    ppLogicalDevice->FreeMemory(poVertexBufferMemory, NULL);
}

void cVertexBuffer::CreateBuffer(cLogicalDevice* pLogicalDevice,
                                 cPhysicalDevice* pPhysicalDevice,
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
    if (!pLogicalDevice->CreateBuffer(&tBufferInfo, NULL, &oBuffer))
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
    tAllocInfo.memoryTypeIndex = FindMemoryType(tMemRequirements.memoryTypeBits, uiProperties, pPhysicalDevice);

    // Allocate the memory for the buffer
    if (!pLogicalDevice->AllocateMemory(&tAllocInfo, NULL, &oBufferMemory))
    {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    // Bind the allocated memory to the buffer
    pLogicalDevice->BindBufferMemory(oBuffer, oBufferMemory, 0);
}

uint cVertexBuffer::FindMemoryType(uint uiTypeFilter,
                                   VkMemoryPropertyFlags tProperties,
                                   cPhysicalDevice* pPhysicalDevice)
{
    // Get memory properties from the physical device
    VkPhysicalDeviceMemoryProperties memProperties;
    pPhysicalDevice->GetPhysicalMemoryProperties(&memProperties);

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

void cVertexBuffer::CopyBuffer(cLogicalDevice* pLogicalDevice,
                               VkBuffer& oSrcBuffer,
                               VkBuffer& oDstBuffer,
                               VkDeviceSize ulSize,
                               VkCommandPool& oCommandPool)
{
    // Struct with information on how to allocate the command buffer
    VkCommandBufferAllocateInfo tAllocInfo = {};
    tAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    tAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    tAllocInfo.commandPool = oCommandPool;
    tAllocInfo.commandBufferCount = 1;

    // Allocate the command buffer
    VkCommandBuffer oCommandBuffer;
    pLogicalDevice->AllocateCommandBuffers(&tAllocInfo, &oCommandBuffer);

    // Struct with details about the usage of this buffer
    VkCommandBufferBeginInfo tBeginInfo = {};
    tBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    tBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // we just want to use it once

    // Struct with information about the copy operation
    VkBufferCopy tCopyRegion = {};
    tCopyRegion.srcOffset = 0;
    tCopyRegion.dstOffset = 0;
    tCopyRegion.size = ulSize;

    // Begin recording the command buffer
    vkBeginCommandBuffer(oCommandBuffer, &tBeginInfo);
    {
        // Copy Buffer operation
        vkCmdCopyBuffer(oCommandBuffer, oSrcBuffer, oDstBuffer, 1, &tCopyRegion);
    }
    // End recording the command buffer
    vkEndCommandBuffer(oCommandBuffer);

    // Struct with information about the buffer we want to submit
    VkSubmitInfo tSubmitInfo = {};
    tSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    tSubmitInfo.commandBufferCount = 1;
    tSubmitInfo.pCommandBuffers = &oCommandBuffer;

    // Submit the command buffer and wait until it finishes
    VkFence oFence = VK_NULL_HANDLE;
    pLogicalDevice->GraphicsQueueSubmit(1, &tSubmitInfo, oFence);
    pLogicalDevice->GraphicsQueueWaitIdle();

    // Free the command buffer once we're done
    pLogicalDevice->FreeCommandBuffers(oCommandPool, 1, &oCommandBuffer);
}

uint cVertexBuffer::GetVertexCount(void)
{
    return patVERTICES.size();
}

void cVertexBuffer::CmdBindVertexBuffer(VkCommandBuffer& oCommandBuffer)
{
    VkBuffer aoVertexBuffers[] = {poVertexBuffer};
    VkDeviceSize aulOffsets[] = {0};

    vkCmdBindVertexBuffers(oCommandBuffer, 0, 1, aoVertexBuffers, aulOffsets);
}
