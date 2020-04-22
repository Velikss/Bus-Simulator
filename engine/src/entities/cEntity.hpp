#pragma once

#include <pch.hpp>
#include <objects/BaseObject.hpp>
#include <entities/cBehaviourHandler.hpp>

class cEntity : public BaseObject
{
public:
    std::vector<cBehaviourHandler *> paBehaviourHandlers;

    cEntity(Mesh *mesh) : BaseObject(mesh)
    {

    }

    void AddBehaviour(cBehaviourHandler *&poBehaviour)
    {
        paBehaviourHandlers.push_back(poBehaviour);
    }

    virtual void Update()
    {
        for (auto &cBehaviourHandler : paBehaviourHandlers)
        {
            cBehaviourHandler->Update(this);
        }
    }
};

