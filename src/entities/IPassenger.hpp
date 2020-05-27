#pragma once

#include <pch.hpp>
#include <vulkan/entities/cEntity.hpp>

class IPassenger : public cEntity
{
public:
    IPassenger(cMesh *mesh) : cEntity(mesh){}
};