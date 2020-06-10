#pragma once

#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>

class cLightObject : public cBaseObject
{
private:
    glm::vec3 ptColor;
    float pfRadius;

public:
    cLightObject(cMesh* pMesh, glm::vec3 tColor, float fRadius, cCollider* pCollider = nullptr, bool bStatic = true);

    glm::vec3 GetColor();
    void SetColor(const glm::vec3& oColor);

    float GetRadius();
    void SetRadius(float fRadius);
};

cLightObject::cLightObject(cMesh* pMesh, glm::vec3 tColor, float fRadius, cCollider* pCollider, bool bStatic)
        : cBaseObject(pMesh, pCollider, bStatic)
{
    assert(fRadius > 0);

    ptColor = tColor;
    pfRadius = fRadius;
}

glm::vec3 cLightObject::GetColor()
{
    return ptColor;
}

float cLightObject::GetRadius()
{
    return pfRadius;
}

void cLightObject::SetColor(const glm::vec3& oColor)
{
    ptColor = oColor;
}

void cLightObject::SetRadius(float fRadius)
{
    pfRadius = fRadius;
}
