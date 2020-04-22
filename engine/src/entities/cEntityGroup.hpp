#pragma once

#include <pch.hpp>
#include <entities/cEntity.hpp>

class cEntityGroup : public cEntity
{
    std::vector<cEntity*> poEntities;
public:
    cEntityGroup() : cEntity(nullptr)
    {

    }

    void AddEntity(cEntity* pEntity)
    {
        poEntities.push_back(pEntity);
    }

    void RemoveEntity()

    void Render()
    {
        for(auto& entity : poEntities)
        {
            entity->Update();
        }
    }

};

