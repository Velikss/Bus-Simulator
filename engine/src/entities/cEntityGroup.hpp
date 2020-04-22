#pragma once

#include <pch.hpp>
#include <entities/cEntity.hpp>

class cEntityGroup : public cEntity
{
    std::vector<cEntity *> poEntities;
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

    void Render()
    {
        for (auto &entity : poEntities)
        {
            entity->Update();
        }
    }
};

