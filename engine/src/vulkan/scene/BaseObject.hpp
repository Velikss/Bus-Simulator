#pragma once

#include <pch.hpp>
#include <vulkan/mesh/Mesh.hpp>
#include <vulkan/collision/Collider.hpp>
#include <vulkan/collision/ColliderSet.hpp>

class cBaseObject
{
private:
    cMesh* ppMesh;
    bool pbStatic;

    cColliderSet* ppColliders;

    glm::vec3 poRotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 poPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 poScale = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::mat4 poModelMatrix = glm::mat4(1);

public:
    cCollider* ppCollider;
    // When set to false, this object will skip the lighting pass and will only be lit by ambient light
    bool bLighting = true;

    // Index to the uniform buffer for this object
    uint puiUniformIndex;

    // True once this object is fully loaded into the world
    bool pbLoaded = false;

    cBaseObject(cMesh* pMesh, cCollider* pCollider = nullptr, bool bStatic = true);
    virtual ~cBaseObject();

    void Setup(cColliderSet* pColliders);

    glm::mat4 GetModelMatrix();
    cMesh* GetMesh();
    bool IsStatic();
    cCollider* GetCollider();

    void SetRotation(const glm::vec3 poRotation);
    void SetPosition(const glm::vec3 poPosition);
    void SetScale(const glm::vec3 poScale);
    glm::vec3 GetPosition();
    glm::vec3 GetRotation();
    virtual void RotateLeft(float fAngleDiff);
    virtual void RotateRight(float fAngleDiff);

private:
    static glm::mat4 CalculateModelMatrix(const glm::vec3& tPosition,
                                          const glm::vec3& tScale,
                                          const glm::vec3& tRotation);
    static glm::mat4 CalculateRotationMatrix(const glm::mat4& tBaseMatrix,
                                             const glm::vec3& tRotation);
};

cBaseObject::cBaseObject(cMesh* pMesh, cCollider* pCollider, bool bStatic) : ppMesh(pMesh)
{
    pbStatic = bStatic;
    ppCollider = pCollider;
}

glm::mat4 cBaseObject::GetModelMatrix()
{
    return poModelMatrix;
}

cMesh* cBaseObject::GetMesh()
{
    return ppMesh;
}

void cBaseObject::SetRotation(const glm::vec3 oRotation)
{
    if (pbStatic && pbLoaded) return;

    if (!pbStatic && pbLoaded && ppCollider != nullptr)
    {
        glm::mat4 tMatrix = CalculateModelMatrix(poPosition, poScale, oRotation);
        if (ppColliders->Collides(ppCollider, tMatrix))
        {
            return;
        }
        poModelMatrix = tMatrix;
    }

    poRotation.x = oRotation.x;
    poRotation.y = oRotation.y;
    poRotation.z = oRotation.z;

    if (pbStatic || !pbLoaded) poModelMatrix = CalculateModelMatrix(poPosition, poScale, poRotation);
}

void cBaseObject::SetPosition(const glm::vec3 oPosition)
{
    if (pbStatic && pbLoaded) return;

    if (!pbStatic && pbLoaded && ppCollider != nullptr)
    {
        glm::mat4 tMatrix = CalculateModelMatrix(oPosition, poScale, poRotation);
        if (ppColliders->Collides(ppCollider, tMatrix))
        {
            return;
        }
        poModelMatrix = tMatrix;
        ppCollider->MarkShouldUpdate();
    }

    poPosition.x = oPosition.x;
    poPosition.y = oPosition.y;
    poPosition.z = oPosition.z;

    if (pbStatic || !pbLoaded) poModelMatrix = CalculateModelMatrix(poPosition, poScale, poRotation);
}

void cBaseObject::SetScale(const glm::vec3 oScale)
{
    if (pbStatic && pbLoaded) return;

    poScale.x = oScale.x;
    poScale.y = oScale.y;
    poScale.z = oScale.z;

    if (pbStatic || !pbLoaded) poModelMatrix = CalculateModelMatrix(poPosition, poScale, poRotation);
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
    if (pbStatic && pbLoaded) return;

    if (poRotation.y >= 360.0f)
        poRotation.y = 0;
    poRotation.y += fAngleDiff;
}

void cBaseObject::RotateRight(float fAngleDiff)
{
    if (pbStatic && pbLoaded) return;

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

glm::mat4 cBaseObject::CalculateModelMatrix(const glm::vec3& tPosition,
                                            const glm::vec3& tScale,
                                            const glm::vec3& tRotation)
{
    glm::mat4 tMatrix(1);

    // Translation
    tMatrix = glm::translate(tMatrix, tPosition);

    // Scaling
    tMatrix = glm::scale(tMatrix, tScale);

    // Rotation
    tMatrix = CalculateRotationMatrix(tMatrix, tRotation);

    return tMatrix;
}

glm::mat4 cBaseObject::CalculateRotationMatrix(const glm::mat4& tBaseMatrix,
                                               const glm::vec3& tRotation)
{
    glm::mat4 tMatrix = tBaseMatrix;

    // Rotation around X axis
    if (tRotation.x > 0.0)
    {
        const glm::vec3 oRot_x = glm::vec3(1.0f, 0.0f, 0.0f);
        tMatrix = glm::rotate(tMatrix, glm::radians(tRotation.x), oRot_x);
    }

    // Rotation around Y axis
    if (tRotation.y > 0.0)
    {
        const glm::vec3 oRot_y = glm::vec3(0.0f, 1.0f, 0.0f);
        tMatrix = glm::rotate(tMatrix, glm::radians(tRotation.y), oRot_y);
    }

    // Rotation around Z axis
    if (tRotation.z > 0.0)
    {
        const glm::vec3 oRot_z = glm::vec3(0.0f, 0.0f, 1.0f);
        tMatrix = glm::rotate(tMatrix, glm::radians(tRotation.z), oRot_z);
    }

    return tMatrix;
}

void cBaseObject::Setup(cColliderSet* pColliders)
{
    ppColliders = pColliders;

    if (ppCollider != nullptr)
    {
        ppCollider->ptWorldPosition = poPosition;
    }
}

cCollider* cBaseObject::GetCollider()
{
    return ppCollider;
}

typedef cBaseObject cModel;
