#pragma once

#include <pch.hpp>
#include <vulkan/entities/Entity.hpp>

class IPassenger : public cEntity
{
public:
    IPassenger(cMesh *mesh) : cEntity(mesh, true){}
};