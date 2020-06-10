#pragma once

#include <pch.hpp>
#include <vulkan/geometry/Geometry.hpp>

class cViewportQuadGeometry : public cGeometry
{
public:
    static cViewportQuadGeometry* GEOMETRY;

    static void Init(cLogicalDevice* pLogicalDevice);
    static void Destroy(cLogicalDevice* pLogicalDevice);
    void LoadIntoRAM() override;

private:
    cViewportQuadGeometry(cLogicalDevice* pLogicalDevice);

    static Vertex CreateVertex(glm::vec3 tPosition, glm::vec2 tTextureCoord);
};

cViewportQuadGeometry* cViewportQuadGeometry::GEOMETRY;

void cViewportQuadGeometry::Init(cLogicalDevice* pLogicalDevice)
{
    GEOMETRY = new cViewportQuadGeometry(pLogicalDevice);
}

void cViewportQuadGeometry::Destroy(cLogicalDevice* pLogicalDevice)
{
    delete GEOMETRY;
    GEOMETRY = nullptr;
}

cViewportQuadGeometry::cViewportQuadGeometry(cLogicalDevice* pLogicalDevice)
        : cGeometry(pLogicalDevice, "internal/viewport_quad", glm::vec2(0))
{
    // Four vertices at the corners of the viewport
    patVertices.emplace_back(CreateVertex(
            {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f} // 0: bottom right
    ));
    patVertices.emplace_back(CreateVertex(
            {-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f} // 1: bottom left
    ));
    patVertices.emplace_back(CreateVertex(
            {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f} // 2: top left
    ));
    patVertices.emplace_back(CreateVertex(
            {1.0f, -1.0f, 0.0f}, {1.0f, 0.0f} // 3: top right
    ));

    // We want to draw two triangles to create the quad
    uint auiIndices[6] = {0, 1, 2, 2, 3, 0};
    for (uint uiIndex : auiIndices)
    {
        paiIndices.push_back(uiIndex);
    }

    // Get the amount of vertices and indices
    puiVertexCount = (uint) patVertices.size();
    puiIndexCount = (uint) paiIndices.size();

    poIndexBuffer = VK_NULL_HANDLE;
    poVertexBuffer = VK_NULL_HANDLE;

    // Setup the buffers on the device and copy the data there
    CopyIntoGPU();

    // Clear the vertices and indices now that they've been loaded on the device
    patVertices.clear();
    paiIndices.clear();
}

void cViewportQuadGeometry::LoadIntoRAM()
{
}

Vertex cViewportQuadGeometry::CreateVertex(glm::vec3 tPosition, glm::vec2 tTextureCoord)
{
    Vertex tVertex = {};
    tVertex.pos = tPosition;
    tVertex.texCoord = tTextureCoord;
    return tVertex;
}
