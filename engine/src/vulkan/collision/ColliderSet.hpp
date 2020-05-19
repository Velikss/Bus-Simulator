#pragma once

#include <pch.hpp>
#include <vulkan/collision/Collider.hpp>

class cColliderSet
{
public:
    std::vector<cCollider*> papColliders;

    bool Collides(cCollider* pCollider, glm::mat4& tColliderMatrix);
};

bool cColliderSet::Collides(cCollider* pCollider, glm::mat4& tColliderMatrix)
{
    uint uiBaseDist = pCollider->GetSize() + 2;
    for (cCollider* pWorldCollider : papColliders)
    {
        if (pWorldCollider != pCollider)
        {
            uint uiMinDist = pWorldCollider->GetSize() + uiBaseDist;
            uint uiDist = (uint) glm::distance(pWorldCollider->ptWorldPosition, pCollider->ptWorldPosition);
            //if (uiDist <= uiMinDist)
            {
                if (pCollider->Collides(tColliderMatrix, pWorldCollider))
                {
                    return true;
                }
            }
        }
    }

    return false;
}
