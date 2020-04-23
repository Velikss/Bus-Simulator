#pragma once

#include <pch.hpp>
#include <entities/cEntity.hpp>

class cEntityGroup : public cEntity
{
    std::vector<cEntity *> poEntities;
    std::vector<cBehaviourHandler *> paBehaviourHandlers;
public:
    cEntityGroup() : cEntity(nullptr)
    {

    }

    void AddEntity(cEntity *pEntity)
    {
        poEntities.push_back(pEntity);
    }

    void RemoveEntity(cEntity *pEntity)
    {
        poEntities.erase(std::remove(poEntities.begin(), poEntities.end(), pEntity));
    }

    void AddBehaviour(cBehaviourHandler *&poBehaviour)
    {
        paBehaviourHandlers.push_back(poBehaviour);
    }

    void RemoveBehaviour(cBehaviourHandler *poBehaviour)
    {
        paBehaviourHandlers.erase(std::remove(paBehaviourHandlers.begin(), paBehaviourHandlers.end(), poBehaviour));
    }

    void UpdateEntities()
    {
        for (auto &entity : poEntities)
        {
            for (auto &cBehaviourHandler : paBehaviourHandlers)
            {
                cBehaviourHandler->Update(entity, this);
            }
        }
    }
};

