#pragma once

#include <pch.hpp>
#include <vulkan/entities/cBehaviourHandler.hpp>
#include <vulkan/entities/cEntityInterface.hpp>

class cEntity : public cEntityInterface
{
public:
    std::vector<cBehaviourHandler *> paBehaviourHandlers;
    glm::vec2 poVelocity;
    glm::vec2 poHeading;
    glm::vec2 poSteeringForce;
    float pfMass;
    float pfMaxSpeed;

    cEntity(cMesh *mesh) : cEntityInterface(mesh)
    {
        poVelocity = glm::vec2(0, 0);
        poHeading = glm::vec2(0, 0);
        poSteeringForce = glm::vec2(0, 0);
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
        *heading = poHeading;
    }

    void ReturnVelocity(glm::vec2 *velocity) override
    {
        *velocity = poVelocity;
    }

    void SetVelocity(glm::vec2 *velocity)
    {
        poVelocity = *velocity;
    }

    void SetHeading(glm::vec2 *heading)
    {
        poHeading = *heading;
    }

    void AppendSteeringForce(glm::vec2 *SteeringForce)
    {
        poSteeringForce += *SteeringForce;
    }

    virtual void Update()
    {
        poSteeringForce = glm::vec2(0, 0);
        for (auto &cBehaviourHandler : paBehaviourHandlers)
        {
            // Runs JavaScript which calculates a steering force and appends it to the current force.
            cBehaviourHandler->Update(this);
        }

        glm::vec2 acceleration = poSteeringForce / pfMaxSpeed;
        poVelocity += acceleration;
        if(poVelocity.length() > pfMaxSpeed)
        {
            poVelocity = glm::normalize(poVelocity);
            poVelocity = poVelocity * pfMaxSpeed;
        }
        poPosition.x += poVelocity.x;
        poPosition.z += poVelocity.y;
        poVelocity *= 0.9;
    }
};

