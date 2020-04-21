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

    VkBufferCreateInfo ptBufferInfo;

public:
    cVertexBuffer(cLogicalDevice* pLogicalDevice, cPhysicalDevice* pPhysicalDevice);
    ~cVertexBuffer(void);

    uint GetVertexCount(void);

    void CmdBindVertexBuffer(VkCommandBuffer& oCommandBuffer);

private:
    void CreateBuffer(cLogicalDevice* pLogicalDevice);
    void AllocateMemory(cLogicalDevice* pLogicalDevice, cPhysicalDevice* pPhysicalDevice);
    void FillBuffer(cLogicalDevice* pLogicalDevice);

    uint FindMemoryType(uint uiTypeFilter, VkMemoryPropertyFlags tProperties, cPhysicalDevice* pPhysicalDevice);
};

cVertexBuffer::cVertexBuffer(cLogicalDevice* pLogicalDevice, cPhysicalDevice* pPhysicalDevice)
{
    ppLogicalDevice = pLogicalDevice;

    CreateBuffer(pLogicalDevice);
    AllocateMemory(pLogicalDevice, pPhysicalDevice);
    FillBuffer(pLogicalDevice);
}

cVertexBuffer::~cVertexBuffer(void)
{
    ppLogicalDevice->DestroyBuffer(poVertexBuffer, NULL);
    ppLogicalDevice->FreeMemory(poVertexBufferMemory, NULL);
}

void cVertexBuffer::CreateBuffer(cLogicalDevice* pLogicalDevice)
{
    // Struct with information about the vertex buffer
    VkBufferCreateInfo tBufferInfo = {};
    tBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    // Set the buffer size to the required size for our vertices
    tBufferInfo.size = sizeof(patVERTICES[0]) * patVERTICES.size();

    // Set the type of the buffer (vertex buffer in this case)
    tBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    // Buffers can be shared between queue families, we're not using this, so set to EXCLUSIVE
    tBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    // Create the vertex buffer
    if (!pLogicalDevice->CreateBuffer(&tBufferInfo, NULL, &poVertexBuffer))
    {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    ptBufferInfo = tBufferInfo;
}

void cVertexBuffer::AllocateMemory(cLogicalDevice* pLogicalDevice, cPhysicalDevice* pPhysicalDevice)
{
    // Struct with memory requirements for the vertex buffer
    VkMemoryRequirements tMemRequirements;
    vkGetBufferMemoryRequirements(pLogicalDevice->GetDevice(), poVertexBuffer, &tMemRequirements);

    // Struct with information on how we want to allocate the buffer
    VkMemoryAllocateInfo tAllocInfo = {};
    tAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    // Set the required memory size
    tAllocInfo.allocationSize = tMemRequirements.size;

    // Find the right memory type and set it
    tAllocInfo.memoryTypeIndex = FindMemoryType(tMemRequirements.memoryTypeBits,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                pPhysicalDevice);

    // Allocate the memory for the buffer
    if (!pLogicalDevice->AllocateMemory(&tAllocInfo, NULL, &poVertexBufferMemory))
    {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    // Bind the allocated memory to the buffer
    pLogicalDevice->BindBufferMemory(poVertexBuffer, poVertexBufferMemory, 0);
}

void cVertexBuffer::FillBuffer(cLogicalDevice* pLogicalDevice)
{
    // Pointer for mapped memory
    void* pData;

    // Map the memory into CPU accessible memory
    pLogicalDevice->MapMemory(poVertexBufferMemory, 0, ptBufferInfo.size, 0, &pData);
    {
        // Copy the vertex data to the mapped memory
        memcpy(pData, patVERTICES.data(), (size_t) ptBufferInfo.size);
    }
    // Unmap the memory again
    pLogicalDevice->UnmapMemory(poVertexBufferMemory);

    // When using a memory type that is not host coherent, memory will have to be flushed here
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
