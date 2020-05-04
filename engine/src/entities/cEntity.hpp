#pragma once

#include <pch.hpp>
#include <objects/BaseObject.hpp>
#include <entities/cBehaviourHandler.hpp>
#include <entities/cEntityInterface.hpp>

class cEntity : public cEntityInterface
{
public:
    std::vector<cBehaviourHandler *> paBehaviourHandlers;
    glm::vec2 pVelocity;
    glm::vec2 pHeading;
    glm::vec2 pSteeringForce;
    float pfMass;
    float pfMaxSpeed;

    cEntity(Mesh *mesh) : cEntityInterface(mesh)
    {
        pVelocity = glm::vec2(0, 0);
        pHeading = glm::vec2(0, 0);
        pSteeringForce = glm::vec2(0, 0);
        pfMass = 1;
        pfMaxSpeed = 0;
    }

    void AddBehaviour(cBehaviourHandler *&poBehaviour)
    {
        paBehaviourHandlers.push_back(poBehaviour);
    }

    void ReturnMass(float *mass) override
    {
        *mass = pfMass;
    }

    void ReturnMaxSpeed(float *speed) override
    {
        *speed = pfMaxSpeed;
    }

    void ReturnHeading(glm::vec2 *heading) override
    {
        *heading = pHeading;
    }

    void ReturnVelocity(glm::vec2 *velocity) override
    {
        *velocity = pVelocity;
    }

    void SetVelocity(glm::vec2 *velocity)
    {
        pVelocity = *velocity;
    }

    void SetHeading(glm::vec2 *heading)
    {
        pHeading = *heading;
    }

    virtual void Update()
    {
        for (auto &cBehaviourHandler : paBehaviourHandlers)
        {
            pSteeringForce += cBehaviourHandler->Update(this);
        }
    }
};

