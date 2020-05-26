#pragma once

#include <pch.hpp>
#include "cEntity.hpp"

class IPassenger : public cEntity
{
public:
    IPassenger(cMesh *mesh) : cEntity(mesh){}
};