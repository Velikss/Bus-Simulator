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
    return new cCollider(cCollisionHelper::UnitRectangle(fScalar));
}
