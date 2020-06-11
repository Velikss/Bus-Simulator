#pragma once

#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>
#include <objects/BusStop.hpp>


class cPassenger : public IPassenger
{
private:
    cBusStop* ppoDestination = nullptr;

public:
    cPassenger(cMesh *mesh) : IPassenger(mesh){}

    void SetDestination(cBusStop* oBusStop);
    cBusStop GetDestination();
    bool DestinationEqual(cBusStop* oBusStop);
};

void cPassenger::SetDestination(cBusStop *oBusStop)
{
    ppoDestination = oBusStop;
}

cBusStop cPassenger::GetDestination()
{
    return *ppoDestination;
}

bool cPassenger::DestinationEqual(cBusStop* oBusStop)
{
    return (oBusStop == ppoDestination);
}