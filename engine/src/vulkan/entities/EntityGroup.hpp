#pragma once

#include <pch.hpp>
#include <vulkan/entities/Entity.hpp>

class cEntityGroup : public IEntityGroup
{
private:
    std::vector<cEntity*> poEntities;
    std::vector<cBehaviourHandler*> paBehaviourHandlers;
public:
    cEntityGroup() : IEntityGroup()
    {

    }

    void AddEntity(cEntity* pEntity);

    void RemoveEntity(cEntity* pEntity);

    std::vector<IBaseEntity*>* GetEntities() override;

    void AddBehaviour(cBehaviourHandler*& poBehaviour);

    void RemoveBehaviour(cBehaviourHandler* poBehaviour);

    void UpdateEntities();

    void GetEntityList(std::vector<IEntity*>** entities) override;

    bool BehaviourExists(cBehaviourHandler* cbBehaviour);

    void ClearEntities();
};

void cEntityGroup::AddEntity(cEntity* pEntity)
{
    poEntities.push_back(pEntity);
}

void cEntityGroup::RemoveEntity(cEntity* pEntity)
{
    poEntities.erase(std::remove(poEntities.begin(), poEntities.end(), pEntity));
}

std::vector<IBaseEntity*>* cEntityGroup::GetEntities()
{
    return reinterpret_cast<std::vector<IBaseEntity*>*>(&poEntities);
}

void cEntityGroup::GetEntityList(std::vector<IEntity*>** entities)
{
    *entities = reinterpret_cast<std::vector<IEntity*>*>(&poEntities);
}

void cEntityGroup::AddBehaviour(cBehaviourHandler*& poBehaviour)
{
    paBehaviourHandlers.push_back(poBehaviour);
}

void cEntityGroup::RemoveBehaviour(cBehaviourHandler* poBehaviour)
{
    paBehaviourHandlers.erase(std::remove(paBehaviourHandlers.begin(), paBehaviourHandlers.end(), poBehaviour));
}

bool cEntityGroup::BehaviourExists(cBehaviourHandler* cbBehaviour)
{
    std::vector<cBehaviourHandler*>::iterator it;
    it = find(paBehaviourHandlers.begin(), paBehaviourHandlers.end(), cbBehaviour);
    if (it != paBehaviourHandlers.end())
        return true;
    else
        return false;
}

void cEntityGroup::ClearEntities()
{
    poEntities.clear();
}

void cEntityGroup::UpdateEntities()
{
    if (!paBehaviourHandlers.empty())
    {
        for (auto& entity : poEntities)
        {
            entity->SetSteeringForce(glm::vec2(0, 0));
            for (auto& cBehaviourHandler : paBehaviourHandlers)
            {
                cBehaviourHandler->Update(entity, this);
            }
            entity->UpdatePosition();
        }
    }
}


