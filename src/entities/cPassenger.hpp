#pragma once

#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>
#include <objects/cBusStop.hpp>


class cPassenger : public IPassenger
{
private:
    cBusStop* ppoDestination = nullptr;

public:
    cPassenger(cMesh *mesh) : IPassenger(mesh){}

    void SetDestination(cBusStop* oBusStop);
    cBusStop GetDestination();
};

void cPassenger::SetDestination(cBusStop *oBusStop)
{
    ppoDestination = oBusStop;
}

cBusStop cPassenger::GetDestination()
{
    return *ppoDestination;
}