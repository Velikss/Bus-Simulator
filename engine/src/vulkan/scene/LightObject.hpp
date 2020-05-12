#pragma once

#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>

class cLightObject : public cBaseObject
{
private:
    glm::vec3 ptColor;
    float pfRadius;

public:
    cLightObject(cMesh* pMesh, glm::vec3 tColor, float fRadius);

    glm::vec3 GetColor();
    float GetRadius();
};

cLightObject::cLightObject(cMesh* pMesh, glm::vec3 tColor, float fRadius) : cBaseObject(pMesh)
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
