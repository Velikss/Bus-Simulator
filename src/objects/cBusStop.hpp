#pragma once

#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>

class cBusStop : public cBaseObject
{
public:

    cBusStop(cMesh *mesh) : cBaseObject(mesh, cCollider::RectangleCollider(-0.8f, -0.8f, 2.8f, 0.8f))
    {
    }

};