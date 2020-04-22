#pragma once

#include <pch.hpp>
#include <objects/BaseObject.hpp>
#include "cBehaviourHandler.hpp"
//#include <entities/cBehaviourHandler.hpp>

class cEntity : public BaseObject
{
public:
    std::vector<cBehaviourHandler *> paBehaviourHandlers;

    cEntity(Mesh *mesh) : BaseObject(mesh)
    {

    }

    virtual void Update()
    {
        // update logic
    }
};

