#pragma once

#include <pch.hpp>
#include <vulkan/mesh/Mesh.hpp>

class cBaseObject
{
private:
    cMesh* ppMesh;

    glm::vec3 poRotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 poPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 poScale = glm::vec3(1.0f, 1.0f, 1.0f);

public:
    cBaseObject(cMesh* pMesh);

    glm::mat4 GetModelMatrix();
    cMesh* GetMesh();

    void setRotation(const glm::vec3 poRotation);
    void setPosition(const glm::vec3 poPosition);
    void setScale(const glm::vec3 poScale);
    glm::vec3* getPosition();
    void MoveForward(float fMultiplier);
    void MoveBackward(float fMultiplier);
    void MoveLeft(float fAngleDiff);
    void MoveRight(float fAngleDiff);

};

cBaseObject::cBaseObject(cMesh* pMesh) : ppMesh(pMesh)
{
    assert(ppMesh != nullptr);
}

glm::mat4 cBaseObject::GetModelMatrix()
{
    glm::mat4 oModel(1.0f);

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

void cBaseObject::setRotation(const glm::vec3 oRotation)
{
    poRotation.x = oRotation.x;
    poRotation.y = oRotation.y;
    poRotation.z = oRotation.z;
}

void cBaseObject::setPosition(const glm::vec3 oPosition)
{
    poPosition.x = oPosition.x;
    poPosition.y = oPosition.y;
    poPosition.z = oPosition.z;
}

void cBaseObject::setScale(const glm::vec3 oScale)
{
    poScale.x = oScale.x;
    poScale.y = oScale.y;
    poScale.z = oScale.z;
}

glm::vec3* cBaseObject::getPosition()
{
    return &poPosition;
}

void cBaseObject::MoveForward(float fMultiplier = 0.01)
{
    glm::vec3 direction(sin(glm::radians(poRotation.y)), 0, cos(glm::radians(poRotation.y)));
    poPosition -= (direction * fMultiplier);
}

 void cBaseObject::MoveBackward(float fMultiplier = 0.01)
{
    glm::vec3 direction(sin(glm::radians(poRotation.y)), 0, cos(glm::radians(poRotation.y)));
    poPosition += (direction * fMultiplier);
}

 void cBaseObject::MoveLeft(float fAngleDiff)
{
    if (poRotation.y >= 360.0f)
        poRotation.y = 0;
    poRotation.y += fAngleDiff;
}

 void cBaseObject::MoveRight(float fAngleDiff)
{
    if (poRotation.y < 0.0f)
        poRotation.y = 360.0f;
    poRotation.y -= fAngleDiff;
}


typedef cBaseObject cModel;
