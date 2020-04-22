#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/Vertex.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/BufferHelper.hpp>

class cVertexBuffer
{
private:
    const std::string MODEL_PATH = "resources/chalet.obj";

    std::vector<Vertex> patVertices;
    std::vector<uint> paiIndices;

    cLogicalDevice* ppLogicalDevice;

    VkBuffer poVertexBuffer;
    VkDeviceMemory poVertexBufferMemory;
    VkBuffer poIndexBuffer;
    VkDeviceMemory poIndexBufferMemory;

public:
    cVertexBuffer(cLogicalDevice* pLogicalDevice,
                  VkCommandPool& oCommandPool);
    ~cVertexBuffer(void);

    void LoadModel(void);

    uint GetVertexCount(void);
    uint GetIndexCount(void);

    void CmdBindVertexBuffer(VkCommandBuffer& oCommandBuffer);
    void CmdBindIndexBuffer(VkCommandBuffer& oCommandBuffer);

private:
    void CreateVertexBuffer(cLogicalDevice* pLogicalDevice,
                            VkCommandPool& oCommandPool);
    void CreateIndexBuffer(cLogicalDevice* pLogicalDevice,
                           VkCommandPool& oCommandPool);

    void CopyBuffer(cLogicalDevice* pLogicalDevice,
                    VkBuffer& oSrcBuffer,
                    VkBuffer& oDstBuffer,
                    VkDeviceSize ulSize,
                    VkCommandPool& oCommandPool);
};

cVertexBuffer::cVertexBuffer(cLogicalDevice* pLogicalDevice,
                             VkCommandPool& oCommandPool)
{
    ppLogicalDevice = pLogicalDevice;

    LoadModel();
    CreateVertexBuffer(pLogicalDevice, oCommandPool);
    CreateIndexBuffer(pLogicalDevice, oCommandPool);
}

cVertexBuffer::~cVertexBuffer(void)
{
    ppLogicalDevice->DestroyBuffer(poVertexBuffer, nullptr);
    ppLogicalDevice->FreeMemory(poVertexBufferMemory, nullptr);

    ppLogicalDevice->DestroyBuffer(poIndexBuffer, nullptr);
    ppLogicalDevice->FreeMemory(poIndexBufferMemory, nullptr);
}

void cVertexBuffer::LoadModel(void)
{
    tinyobj::attrib_t tAttrib;
    std::vector<tinyobj::shape_t> atShapes;
    std::vector<tinyobj::material_t> atMaterials;
    std::string sError;

    if (!tinyobj::LoadObj(&tAttrib, &atShapes, &atMaterials, &sError, MODEL_PATH.c_str()))
    {
        throw std::runtime_error(sError);
    }

    for (const auto& tShape : atShapes)
    {
        for (const auto& index : tShape.mesh.indices)
        {
            Vertex tVertex = {};

            tVertex.pos = {
                    tAttrib.vertices[3 * index.vertex_index + 0],
                    tAttrib.vertices[3 * index.vertex_index + 1],
                    tAttrib.vertices[3 * index.vertex_index + 2]
            };

            // TODO: Add texture support
            /*tVertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
            };*/

            tVertex.color = {1.0f, 1.0f, 1.0f};

            patVertices.push_back(tVertex);
            paiIndices.push_back(paiIndices.size());
        }
    }
}

void cVertexBuffer::CreateVertexBuffer(cLogicalDevice* pLogicalDevice,
                                       VkCommandPool& oCommandPool)
{
    VkDeviceSize ulBufferSize = sizeof(patVertices[0]) * patVertices.size();

    // Create a stagingBuffer which is host visible and coherent so we can copy to it
    VkBuffer oStagingBuffer;
    VkDeviceMemory oStagingBufferMemory;
    cBufferHelper::CreateBuffer(pLogicalDevice, ulBufferSize,
                                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                oStagingBuffer, oStagingBufferMemory);

    // Pointer for mapped memory
    void* pData;

    // Map the memory into CPU accessible memory
    pLogicalDevice->MapMemory(oStagingBufferMemory, 0, ulBufferSize, 0, &pData);
    {
        // Copy the vertex data to the mapped memory
        memcpy(pData, patVertices.data(), (uint) ulBufferSize);
    }
    // Unmap the memory again
    pLogicalDevice->UnmapMemory(oStagingBufferMemory);

    // Create a vertexBuffer which is local to the graphics device
    cBufferHelper::CreateBuffer(pLogicalDevice, ulBufferSize,
                                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                poVertexBuffer, poVertexBufferMemory);

    // Copy the data from the stagingBuffer to the vertexBuffer
    CopyBuffer(pLogicalDevice, oStagingBuffer, poVertexBuffer, ulBufferSize, oCommandPool);

    // Clean up the stagingBuffer
    pLogicalDevice->DestroyBuffer(oStagingBuffer, nullptr);
    pLogicalDevice->FreeMemory(oStagingBufferMemory, nullptr);
}

void cVertexBuffer::CreateIndexBuffer(cLogicalDevice* pLogicalDevice,
                                      VkCommandPool& oCommandPool)
{
    VkDeviceSize ulBufferSize = sizeof(paiIndices[0]) * paiIndices.size();

    VkBuffer oStagingBuffer;
    VkDeviceMemory oStagingBufferMemory;
    cBufferHelper::CreateBuffer(pLogicalDevice, ulBufferSize,
                                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                oStagingBuffer, oStagingBufferMemory);

    // Pointer for mapped memory
    void* pData;

    // Map the memory into CPU accessible memory
    pLogicalDevice->MapMemory(oStagingBufferMemory, 0, ulBufferSize, 0, &pData);
    {
        // Copy the vertex data to the mapped memory
        memcpy(pData, paiIndices.data(), (uint) ulBufferSize);
    }
    // Unmap the memory again
    pLogicalDevice->UnmapMemory(oStagingBufferMemory);

    // Create a vertexBuffer which is local to the graphics device
    cBufferHelper::CreateBuffer(pLogicalDevice, ulBufferSize,
                                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                poIndexBuffer, poIndexBufferMemory);

    // Copy the data from the stagingBuffer to the vertexBuffer
    CopyBuffer(pLogicalDevice, oStagingBuffer, poIndexBuffer, ulBufferSize, oCommandPool);

    // Clean up the stagingBuffer
    pLogicalDevice->DestroyBuffer(oStagingBuffer, nullptr);
    pLogicalDevice->FreeMemory(oStagingBufferMemory, nullptr);
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
    return patVertices.size();
}

uint cVertexBuffer::GetIndexCount(void)
{
    return paiIndices.size();
}

void cVertexBuffer::CmdBindVertexBuffer(VkCommandBuffer& oCommandBuffer)
{
    VkBuffer aoVertexBuffers[] = {poVertexBuffer};
    VkDeviceSize aulOffsets[] = {0};

    vkCmdBindVertexBuffers(oCommandBuffer, 0, 1, aoVertexBuffers, aulOffsets);
}

void cVertexBuffer::CmdBindIndexBuffer(VkCommandBuffer& oCommandBuffer)
{
    vkCmdBindIndexBuffer(oCommandBuffer, poIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
}
