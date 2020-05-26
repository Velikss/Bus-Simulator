#pragma once

#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>
#include "cBusStop.hpp"


class cPassenger : public IPassenger
{
private:
    cBusStop* ppoDestination;

public:
    cPassenger(cMesh *mesh) : IPassenger(mesh){}
};