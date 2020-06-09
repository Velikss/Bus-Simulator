#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/geometry/Vertex.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/geometry/BufferHelper.hpp>
#include <vulkan/geometry/ModelHelper.hpp>

// Class representing some geometry (vertices) that can be rendered in the scene
class cGeometry
{
protected:
    // Vertices and indices of this geometry
    std::vector<Vertex> patVertices;
    std::vector<uint> paiIndices;

    uint puiVertexCount;
    uint puiIndexCount;

    glm::vec2 ptUVScale;

    // Device where this geometry is loaded
    cLogicalDevice* ppLogicalDevice;

    string psFilePath;

    // Buffer and memory handles for the vertices and indices
    VkBuffer poVertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory poVertexBufferMemory = VK_NULL_HANDLE;
    VkBuffer poIndexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory poIndexBufferMemory = VK_NULL_HANDLE;

public:
    cGeometry(cLogicalDevice* pLogicalDevice,
              const string& sFilePath,
              const glm::vec2& tUVScale);
    ~cGeometry();

    virtual void LoadIntoRAM();
    void LoadIntoGPU();
    void UnloadFromRAM();
    void UnloadFromGPU();

    // Returns the amount of indices in this geometry
    uint GetIndexCount();

    // Add a command to the command buffer which binds the vertex buffer
    void CmdBindVertexBuffer(VkCommandBuffer& oCommandBuffer);
    // Add a command to the command buffer which binds the index buffer
    void CmdBindIndexBuffer(VkCommandBuffer& oCommandBuffer);
};

cGeometry::cGeometry(cLogicalDevice* pLogicalDevice,
                     const string& sFilePath,
                     const glm::vec2& tUVScale)
{
    assert(pLogicalDevice != nullptr);
    assert(sFilePath.length() > 0);

    ppLogicalDevice = pLogicalDevice;
    psFilePath = sFilePath;
    ptUVScale = tUVScale;
}

cGeometry::~cGeometry()
{
    if (patVertices.size() > 0)
    {
        UnloadFromRAM();
    }
    if (poVertexBuffer != VK_NULL_HANDLE)
    {
        UnloadFromGPU();
    }
}

void cGeometry::LoadIntoRAM()
{
    assert(patVertices.size() == 0);
    assert(paiIndices.size() == 0);

    // Load the model into the vertices and indices lists
    cModelHelper::LoadModel(psFilePath.c_str(), patVertices, paiIndices);

    // Get the amount of vertices and indices
    puiVertexCount = (uint) patVertices.size();
    puiIndexCount = (uint) paiIndices.size();

    assert(puiVertexCount > 0);  // there should be vertices
    assert(puiIndexCount > 0);   // there should be indices

    // If a custom UV scale is specified, rescale the UV's
    if (ptUVScale.x != 1.0f || ptUVScale.y != 1.0f)
    {
        for (Vertex& tVertex : patVertices)
        {
            tVertex.texCoord.x *= ptUVScale.x;
            tVertex.texCoord.y *= ptUVScale.y;
        }
    }
}

void cGeometry::LoadIntoGPU()
{
    assert(ppLogicalDevice != nullptr);       // can't setup buffers on an non-existent device
    assert(poVertexBuffer == VK_NULL_HANDLE);
    assert(poIndexBuffer == VK_NULL_HANDLE);

    // Create and setup the vertex buffer
    VkDeviceSize ulVertexBufferSize = sizeof(patVertices[0]) * patVertices.size();
    cBufferHelper::CopyToNewBuffer(ppLogicalDevice,
                                   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                   patVertices.data(), ulVertexBufferSize,
                                   poVertexBuffer, poVertexBufferMemory);

    // Create and setup the index buffer
    VkDeviceSize ulIndexBufferSize = sizeof(paiIndices[0]) * paiIndices.size();
    cBufferHelper::CopyToNewBuffer(ppLogicalDevice,
                                   VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                   paiIndices.data(), ulIndexBufferSize,
                                   poIndexBuffer, poIndexBufferMemory);
}

void cGeometry::UnloadFromRAM()
{
    assert(patVertices.size() > 0);
    assert(paiIndices.size() > 0);

    patVertices.resize(0);
    paiIndices.resize(0);
}

void cGeometry::UnloadFromGPU()
{
    assert(poVertexBuffer != VK_NULL_HANDLE);
    assert(poIndexBuffer != VK_NULL_HANDLE);

    // Destroy and free the index buffer
    ppLogicalDevice->DestroyBuffer(poIndexBuffer, nullptr);
    ppLogicalDevice->FreeMemory(poIndexBufferMemory, nullptr);

    poIndexBuffer = VK_NULL_HANDLE;
    poIndexBufferMemory = VK_NULL_HANDLE;

    // Destroy and free the vertex buffer
    ppLogicalDevice->DestroyBuffer(poVertexBuffer, nullptr);
    ppLogicalDevice->FreeMemory(poVertexBufferMemory, nullptr);

    poVertexBuffer = VK_NULL_HANDLE;
    poVertexBufferMemory = VK_NULL_HANDLE;
}

uint cGeometry::GetIndexCount(void)
{
    return puiIndexCount;
}

void cGeometry::CmdBindVertexBuffer(VkCommandBuffer& oCommandBuffer)
{
    // We don't want to use any special offset for the buffer
    VkDeviceSize ulOffset = 0;

    vkCmdBindVertexBuffers(oCommandBuffer, 0, 1, &poVertexBuffer, &ulOffset);
}

void cGeometry::CmdBindIndexBuffer(VkCommandBuffer& oCommandBuffer)
{
    vkCmdBindIndexBuffer(oCommandBuffer, poIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
}
