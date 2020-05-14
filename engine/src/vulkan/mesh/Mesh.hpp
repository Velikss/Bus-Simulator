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
    cTexture* ppMaterial;

public:
    cMesh(cGeometry* pGeometry, cTexture* pTexture, cTexture* pMaterial);
    cMesh(cGeometry* pGeometry, cTexture* pTexture);

    cGeometry* GetGeometry();
    cTexture* GetTexture();
    cTexture* GetMaterial();
};

cMesh::cMesh(cGeometry* pGeometry, cTexture* pTexture, cTexture* pMaterial)
{
    assert(pGeometry != nullptr);
    assert(pTexture != nullptr);
    assert(pMaterial != nullptr);

    ppGeometry = pGeometry;
    ppTexture = pTexture;
    ppMaterial = pMaterial;
}

cMesh::cMesh(cGeometry* pGeometry, cTexture* pTexture) : cMesh(pGeometry, pTexture, pTexture)
{
}

cGeometry* cMesh::GetGeometry()
{
    return ppGeometry;
}

cTexture* cMesh::GetTexture()
{
    return ppTexture;
}

cTexture* cMesh::GetMaterial()
{
    return ppMaterial;
}
