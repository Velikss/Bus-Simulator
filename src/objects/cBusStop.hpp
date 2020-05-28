#pragma once

#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>
#include <vulkan/entities/cEntityGroup.hpp>
#include <entities/IPassengerHolder.hpp>

class cBusStop : public cBaseObject, public IPassengerHolder
{
public:
    cEntityGroup* poEntityGroup;
    bool pbVisited;

    cBusStop(cMesh *mesh) : cBaseObject(mesh, cCollider::RectangleCollider(-0.8f, -0.8f, 2.8f, 0.8f))
    {
        poEntityGroup = new cEntityGroup;
        pbVisited = false;
    }

    bool AddPassenger(IPassenger *passenger) override;

    bool RemovePassenger(IPassenger *passenger) override;
};

bool cBusStop::AddPassenger(IPassenger *passenger)
{
    poEntityGroup->AddEntity(passenger);
    return true;
}

bool cBusStop::RemovePassenger(IPassenger *passenger)
{
    poEntityGroup->RemoveEntity(passenger);
    return true;
}
