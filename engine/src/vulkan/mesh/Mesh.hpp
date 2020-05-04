#pragma once

#include <pch.hpp>
#include <vulkan/geometry/Geometry.hpp>
#include <vulkan/texture/Texture.hpp>

// This class is very simple right now, it just connects a geometry and texture
// together. Later, things like materials will be added here.
class cMesh
{
private:
    cGeometry* ppGeometry;
    cTexture* ppTexture;

public:
    cMesh(cGeometry* pGeometry, cTexture* pTexture);

    cGeometry* GetGeometry();
    cTexture* GetTexture();
};

cMesh::cMesh(cGeometry* pGeometry, cTexture* pTexture)
{
    assert(pGeometry != nullptr);
    assert(pTexture != nullptr);

    ppGeometry = pGeometry;
    ppTexture = pTexture;
}

cGeometry* cMesh::GetGeometry()
{
    return ppGeometry;
}

cTexture* cMesh::GetTexture()
{
    return ppTexture;
}
