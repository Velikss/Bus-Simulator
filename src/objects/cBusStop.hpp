#pragma once

#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>
#include <vulkan/entities/cEntityGroup.hpp>
#include <entities/IPassengerHolder.hpp>

class cBusStop : public cLightObject, public IPassengerHolder
{
public:
    cEntityGroup* poEntityGroup;
    bool pbVisited;

    cBusStop(cMesh* mesh) : cLightObject(mesh, glm::vec3(1, 1, 0.2), 25,
                                         cCollider::RectangleCollider(-0.8f, -0.8f, 2.8f, 0.8f))
    {
        poEntityGroup = new cEntityGroup;
        pbVisited = false;
    }

    bool AddPassenger(IPassenger* pPassenger) override;

    bool RemovePassenger(IPassenger* pPassenger) override;

    void SetPassengersTarget(glm::vec3 oTargetPos);
};

bool cBusStop::AddPassenger(IPassenger* pPassenger)
{
    poEntityGroup->AddEntity(pPassenger);
    return true;
}

bool cBusStop::RemovePassenger(IPassenger* pPassenger)
{
    poEntityGroup->RemoveEntity(pPassenger);
    return true;
}

void cBusStop::SetPassengersTarget(glm::vec3 oTargetPos)
{
    std::vector<IEntity*>* entities;
    poEntityGroup->GetEntityList(&entities);
    for (int i = 0; i < poEntityGroup->GetEntities()->size(); i++)
    {
        (*entities)[i]->SetTarget(oTargetPos);
    }
}
