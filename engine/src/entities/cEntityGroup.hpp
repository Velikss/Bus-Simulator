#pragma once

#include <pch.hpp>
#include <entities/cEntity.hpp>

class cEntityGroup : public cEntityGroupInterface
{
    std::vector<cEntity *> poEntities;
    std::vector<cBehaviourHandler *> paBehaviourHandlers;
public:
    cEntityGroup() : cEntityGroupInterface()
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

    void ReturnEntities(std::vector<cEntityInterface *> **entities) override
    {
        *entities = reinterpret_cast<std::vector<cEntityInterface *> *>(&poEntities);
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

