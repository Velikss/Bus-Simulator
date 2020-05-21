#pragma once

#include <pch.hpp>
#include <vulkan/collision/CollisionHelper.hpp>

class cCollider
{
private:
    tRectangle ptCollisionBox;
    tRectangle ptTransformedBox;

    bool pbUpdateTransformed = true;

    uint uiSize;

public:
    glm::vec3 ptWorldPosition;

    cCollider(tRectangle tCollisionBox);

    void MarkShouldUpdate();
    void Update(glm::mat4 tOurMatrix);
    bool Collides(glm::mat4 tOurMatrix, cCollider* pColliderB);
    uint GetSize();

    static cCollider* UnitCollider(float fScalar = 1);
    static cCollider* RectangleCollider(float xScale, float zScale);
    static cCollider* RectangleCollider(float x1, float z1, float x2, float z2);
};

cCollider::cCollider(tRectangle tCollisionBox)
{
    ptCollisionBox = tCollisionBox;

    uiSize = (uint) glm::distance(tCollisionBox.aVertices[0], tCollisionBox.aVertices[1]);
}

void cCollider::MarkShouldUpdate()
{
    pbUpdateTransformed = true;
}

void cCollider::Update(glm::mat4 tOurMatrix)
{
    ptTransformedBox = cCollisionHelper::TransformRectangle(ptCollisionBox, tOurMatrix);
}

bool cCollider::Collides(glm::mat4 tOurMatrix, cCollider* pColliderB)
{
    if (pbUpdateTransformed)
    {
        Update(tOurMatrix);
    }

    return cCollisionHelper::Collides(ptTransformedBox, pColliderB->ptTransformedBox);
}

uint cCollider::GetSize()
{
    return uiSize;
}

cCollider* cCollider::UnitCollider(float fScalar)
{
    return RectangleCollider(fScalar, fScalar);
}

cCollider* cCollider::RectangleCollider(float xScale, float zScale)
{
    tRectangle tRectangle = {};
    tRectangle.aVertices[0] = glm::vec2(0, 0);
    tRectangle.aVertices[1] = glm::vec2(0, -zScale);
    tRectangle.aVertices[2] = glm::vec2(xScale, -zScale);
    tRectangle.aVertices[3] = glm::vec2(xScale, 0);
    return new cCollider(tRectangle);
}

cCollider* cCollider::RectangleCollider(float x1, float z1, float x2, float z2)
{
    tRectangle tRectangle = {};
    tRectangle.aVertices[0] = glm::vec2(x1, z1);
    tRectangle.aVertices[1] = glm::vec2(x1, z2);
    tRectangle.aVertices[2] = glm::vec2(x2, z2);
    tRectangle.aVertices[3] = glm::vec2(x2, z1);
    return new cCollider(tRectangle);
}
