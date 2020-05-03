#pragma once

#include <pch.hpp>
#include <vulkan/geometry/Geometry.hpp>
#include <vulkan/texture/Texture.hpp>

class cMesh
{
private:
    cGeometry* ppGeometry;
    uint puiTextureIndex;

public:
    cMesh(cGeometry* pGeometry, uint uiTextureIndex);

    cGeometry* GetGeometry();
    uint GetTextureIndex();
};

cMesh::cMesh(cGeometry* pGeometry, uint uiTextureIndex)
{
    assert(pGeometry != nullptr);

    ppGeometry = pGeometry;
    puiTextureIndex = uiTextureIndex;
}

cGeometry* cMesh::GetGeometry()
{
    return ppGeometry;
}

uint cMesh::GetTextureIndex()
{
    return puiTextureIndex;
}
