#pragma once

#include <pch.hpp>
#include <vulkan/mesh/Mesh.hpp>

class cBaseObject
{
private:
    cMesh* ppMesh;
    bool pbStatic;

    glm::vec3 poRotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 poPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 poScale = glm::vec3(1.0f, 1.0f, 1.0f);

public:
    bool pbLighting = true;
    bool pbVisible = true;
    uint puiUniformIndex;

    cBaseObject(cMesh* pMesh, bool bStatic = true);
    virtual ~cBaseObject();

    glm::mat4 GetModelMatrix();
    cMesh* GetMesh();
    bool IsStatic();

    void SetRotation(const glm::vec3 poRotation);
    void SetPosition(const glm::vec3 poPosition);
    void SetScale(const glm::vec3 poScale);
    glm::vec3 GetPosition();
    glm::vec3 GetRotation();
    virtual void RotateLeft(float fAngleDiff);
    virtual void RotateRight(float fAngleDiff);
};

cBaseObject::cBaseObject(cMesh* pMesh, bool bStatic) : ppMesh(pMesh)
{
    pbStatic = bStatic;
}

glm::mat4 cBaseObject::GetModelMatrix()
{
    glm::mat4 oModel(1.0f);

    if (!pbVisible)
    {
        return glm::scale(oModel, glm::vec3(0));
    }

    // TODO: Maybe cache the value and only recalculate when needed

    // Translation
    oModel = glm::translate(oModel, poPosition);

    // Scaling
    oModel = glm::scale(oModel, poScale);

    // Rotation around X axis
    if (poRotation.x > 0.0)
    {
        const glm::vec3 oRot_x = glm::vec3(1.0f, 0.0f, 0.0f);
        oModel = glm::rotate(oModel, glm::radians(poRotation.x), oRot_x);
    }

    // Rotation around Y axis
    if (poRotation.y > 0.0)
    {
        const glm::vec3 oRot_y = glm::vec3(0.0f, 1.0f, 0.0f);
        oModel = glm::rotate(oModel, glm::radians(poRotation.y), oRot_y);
    }

    // Rotation around Z axis
    if (poRotation.z > 0.0)
    {
        const glm::vec3 oRot_z = glm::vec3(0.0f, 0.0f, 1.0f);
        oModel = glm::rotate(oModel, glm::radians(poRotation.z), oRot_z);
    }

    return oModel;
}

cMesh* cBaseObject::GetMesh()
{
    return ppMesh;
}

void cBaseObject::SetRotation(const glm::vec3 oRotation)
{
    poRotation.x = oRotation.x;
    poRotation.y = oRotation.y;
    poRotation.z = oRotation.z;
}

void cBaseObject::SetPosition(const glm::vec3 oPosition)
{
    poPosition.x = oPosition.x;
    poPosition.y = oPosition.y;
    poPosition.z = oPosition.z;
}

void cBaseObject::SetScale(const glm::vec3 oScale)
{
    poScale.x = oScale.x;
    poScale.y = oScale.y;
    poScale.z = oScale.z;
}

glm::vec3 cBaseObject::GetPosition()
{
    return poPosition;
}

glm::vec3 cBaseObject::GetRotation()
{
    return poRotation;
}

void cBaseObject::RotateLeft(float fAngleDiff)
{
    if (poRotation.y >= 360.0f)
        poRotation.y = 0;
    poRotation.y += fAngleDiff;
}

void cBaseObject::RotateRight(float fAngleDiff)
{
    if (poRotation.y < 0.0f)
        poRotation.y = 360.0f;
    poRotation.y -= fAngleDiff;
}

cBaseObject::~cBaseObject()
{
}

bool cBaseObject::IsStatic()
{
    return pbStatic;
}

typedef cBaseObject cModel;
