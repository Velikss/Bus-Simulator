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
private:
    // Vertices and indices of this geometry
    std::vector<Vertex> patVertices;
    std::vector<uint> paiIndices;

    uint puiVertexCount;
    uint puiIndexCount;

    // Device where this geometry is loaded
    cLogicalDevice* ppLogicalDevice;

    // Buffer and memory handles for the vertices and indices
    VkBuffer poVertexBuffer;
    VkDeviceMemory poVertexBufferMemory;
    VkBuffer poIndexBuffer;
    VkDeviceMemory poIndexBufferMemory;

public:
    // Create a new Geometry from an OBJ file
    static cGeometry* FromOBJFile(const char* sFilePath,
                                  cLogicalDevice* pLogicalDevice,
                                  float fXUVScale = 1.0f,
                                  float fYUVScale = 1.0f);

    // Cleans up the buffers and frees the memory on the device which is used for this geometry
    ~cGeometry();

    // Returns the amount of indices in this geometry
    uint GetIndexCount();

    // Add a command to the command buffer which binds the vertex buffer
    void CmdBindVertexBuffer(VkCommandBuffer& oCommandBuffer);
    // Add a command to the command buffer which binds the index buffer
    void CmdBindIndexBuffer(VkCommandBuffer& oCommandBuffer);

private:
    // Setup buffers for the vertices and indices on the device and copy the data to them
    void CopyToDevice(cLogicalDevice* pLogicalDevice);
};

cGeometry* cGeometry::FromOBJFile(const char* sFilePath,
                                  cLogicalDevice* pLogicalDevice,
                                  float fXUVScale, float fYUVScale)
{
    cGeometry* pGeometry = new cGeometry();

    // Load the model into the vertices and indices lists
    cModelHelper::LoadModel(sFilePath, pGeometry->patVertices, pGeometry->paiIndices);

    // Get the amount of vertices and indices
    pGeometry->puiVertexCount = pGeometry->patVertices.size();
    pGeometry->puiIndexCount = pGeometry->paiIndices.size();

    assert(pGeometry->puiVertexCount > 0);  // there should be vertices
    assert(pGeometry->puiIndexCount > 0);   // there should be indices

    // If a custom UV scale is specified, rescale the UV's
    if (fXUVScale != 1.0f || fYUVScale != 1.0f)
    {
        for (Vertex& tVertex : pGeometry->patVertices)
        {
            tVertex.texCoord.x *= fXUVScale;
            tVertex.texCoord.y *= fYUVScale;
        }
    }

    // Setup the buffers on the device and copy the data there
    pGeometry->CopyToDevice(pLogicalDevice);

    // Clear the vertices and indices now that they've been loaded on the device
    pGeometry->patVertices.clear();
    pGeometry->paiIndices.clear();

    return pGeometry;
}

void cGeometry::CopyToDevice(cLogicalDevice* pLogicalDevice)
{
    assert(pLogicalDevice != nullptr);  // can't setup buffers on an non-existent device

    // Store the logical device so we can clean up the buffers in the destructor
    ppLogicalDevice = pLogicalDevice;

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

cGeometry::~cGeometry()
{
    // Destroy and free the index buffer
    ppLogicalDevice->DestroyBuffer(poIndexBuffer, nullptr);
    ppLogicalDevice->FreeMemory(poIndexBufferMemory, nullptr);

    // Destroy and free the vertex buffer
    ppLogicalDevice->DestroyBuffer(poVertexBuffer, nullptr);
    ppLogicalDevice->FreeMemory(poVertexBufferMemory, nullptr);
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
