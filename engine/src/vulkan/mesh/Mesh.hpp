#pragma once

#include <pch.hpp>
#include <vulkan/geometry/Geometry.hpp>
#include <vulkan/texture/Texture.hpp>
#include <vulkan/util/Invalidatable.hpp>

class cMesh : public cInvalidatable
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

    void SetTexture(cTexture* pTexture);
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

void cMesh::SetTexture(cTexture* pTexture)
{
    ppTexture = pTexture;
    Invalidate();
}
