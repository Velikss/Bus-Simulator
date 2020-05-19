#pragma once

#include <pch.hpp>
#include <vulkan/entities/cEntity.hpp>

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
            entity->SetSteeringForce(glm::vec2(0, 0));
            for (auto &cBehaviourHandler : paBehaviourHandlers)
            {
                cBehaviourHandler->Update(entity, this);
            }
            if (!isnan(entity->GetSteeringForce().x) && !isnan(entity->GetSteeringForce().y))
            {
                glm::vec2 acceleration = entity->GetSteeringForce() / entity->GetMaxSpeed();
                entity->SetVelocity(entity->GetVelocity() + acceleration);
                if (entity->GetVelocity().length() > entity->GetMaxSpeed())
                {
                    entity->SetVelocity(glm::normalize(entity->GetVelocity()));
                    entity->SetVelocity(entity->GetVelocity() * entity->GetMaxSpeed());
                }

                glm::vec3 pos = entity->GetPosition();
                pos.x += entity->GetVelocity().x;
                pos.z += entity->GetVelocity().y;
                entity->SetPosition(pos);
                if (entity->GetVelocity().x > 0.001 && entity->GetVelocity().y > 0.001)
                {
                    entity->SetVelocity(entity->GetVelocity() *= 0.9);
                }
            }
        }

    }
};

