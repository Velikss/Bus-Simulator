#pragma once

#include <pch.hpp>
#include <vulkan/entities/cBehaviourHandler.hpp>
#include <vulkan/entities/cEntityInterface.hpp>

class cEntity : public cEntityInterface
{
private:
    std::vector<cBehaviourHandler *> paBehaviourHandlers;
    glm::vec3 poTarget;
    glm::vec2 poVelocity;
    glm::vec2 poHeading;
    glm::vec2 poSteeringForce;
    float pfMass;
    float pfMaxSpeed;
public:
    cEntity(cMesh *mesh) : cEntityInterface(mesh)
    {
        poVelocity = glm::vec2(0, 0);
        poHeading = glm::vec2(0, 0);
        poSteeringForce = glm::vec2(0, 0);
        pfMass = 1;
        pfMaxSpeed = 0.1;
    }

    void AddBehaviour(cBehaviourHandler *&poBehaviour);

    void SetMass(float fMass) override;

    float GetMass() override;

    void SetMaxSpeed(float fSpeed) override;

    float GetMaxSpeed() override;

    void SetHeading(glm::vec2 oHeading) override;

    glm::vec2 GetHeading() override;

    void SetVelocity(glm::vec2 oVelocity) override;

    glm::vec2 GetVelocity() override;

    void SetTarget(glm::vec3 oTarget) override;

    glm::vec3 GetTarget() override;

    glm::vec2 GetSteeringForce() override;

    void SetSteeringForce(glm::vec2 oSteeringForce) override;

    void AppendSteeringForce(glm::vec2 oSteeringForce) override;

    void Update();

};

void cEntity::AddBehaviour(cBehaviourHandler *&poBehaviour)
{
    paBehaviourHandlers.push_back(poBehaviour);
}

void cEntity::SetMass(float fMass)
{
    pfMass = fMass;
}

float cEntity::GetMass()
{
    return pfMass;
}

void cEntity::SetMaxSpeed(float fSpeed)
{
    pfMaxSpeed = fSpeed;
}

float cEntity::GetMaxSpeed()
{
    return pfMaxSpeed;
}

void cEntity::SetHeading(glm::vec2 oHeading)
{
    poHeading = oHeading;
}

glm::vec2 cEntity::GetHeading()
{
    return poHeading;
}


void cEntity::SetVelocity(glm::vec2 oVelocity)
{
    poVelocity = oVelocity;
}

glm::vec2 cEntity::GetVelocity()
{
    return poVelocity;
}

void cEntity::SetSteeringForce(glm::vec2 oSteeringForce)
{
    poSteeringForce = oSteeringForce;
}

glm::vec2 cEntity::GetSteeringForce()
{
    return poSteeringForce;
}

void cEntity::AppendSteeringForce(glm::vec2 oSteeringForce)
{
    poSteeringForce += oSteeringForce;
}

void cEntity::SetTarget(glm::vec3 oTarget)
{
    poTarget = oTarget;
}

glm::vec3 cEntity::GetTarget()
{
    return poTarget;
}

void cEntity::Update()
{
    poSteeringForce = glm::vec2(0, 0);
    for (auto &cBehaviourHandler : paBehaviourHandlers)
    {
        // Runs JavaScript which calculates a steering force and appends it to the current force.
        cBehaviourHandler->Update(this);
    }
    if (!isnan(poSteeringForce.x) && !isnan(poSteeringForce.y))
    {
        glm::vec2 acceleration = poSteeringForce / pfMaxSpeed;
        poVelocity += acceleration;
        if (poVelocity.length() > pfMaxSpeed)
        {
            poVelocity = glm::normalize(poVelocity);
            poVelocity = poVelocity * pfMaxSpeed;
        }
        glm::vec3 pos = GetPosition();
        pos.x += poVelocity.x;
        pos.z += poVelocity.y;
        SetPosition(pos);
        if (poVelocity.x > 0.001 && poVelocity.y > 0.001)
        {
            poVelocity *= 0.9;
        }
    }

}

