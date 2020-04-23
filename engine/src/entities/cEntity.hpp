#pragma once

#include <pch.hpp>
#include <objects/BaseObject.hpp>
#include <entities/cBehaviourHandler.hpp>
#include <entities/cEntityInterface.hpp>

class cEntity : public cEntityInterface
{
public:
    std::vector<cBehaviourHandler *> paBehaviourHandlers;

    cEntity(Mesh *mesh) : cEntityInterface(mesh)
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

