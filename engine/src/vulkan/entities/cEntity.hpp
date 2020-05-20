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
    glm::vec2 poSteeringForce;
    float pfMass = 1;
    float pfMaxSpeed = 0.1;
public:
    cEntity(cMesh *mesh) : cEntityInterface(mesh)
    {
    }

    void AddBehaviour(cBehaviourHandler *&poBehaviour);

    void SetMaxSpeed(float fSpeed) override;

    float GetMaxSpeed() override;

    void SetVelocity(glm::vec2 oVelocity) override;

    glm::vec2 GetVelocity() override;

    void SetTarget(glm::vec3 oTarget) override;

    glm::vec3 GetTarget() override;

    glm::vec2 GetSteeringForce() override;

    void SetSteeringForce(glm::vec2 oSteeringForce) override;

    void AppendSteeringForce(glm::vec2 oSteeringForce) override;

    void Update();

    void UpdatePosition();
};

void cEntity::AddBehaviour(cBehaviourHandler *&poBehaviour)
{
    paBehaviourHandlers.push_back(poBehaviour);
}

void cEntity::SetMaxSpeed(float fSpeed)
{
    pfMaxSpeed = fSpeed;
}

float cEntity::GetMaxSpeed()
{
    return pfMaxSpeed;
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

void cEntity::UpdatePosition()
{
    // Update velocity values
    glm::vec2 acceleration = poSteeringForce / pfMaxSpeed;
    poVelocity += acceleration;
    if (poVelocity.length() > pfMaxSpeed)
    {
        poVelocity = glm::normalize(poVelocity);
        poVelocity = poVelocity * pfMaxSpeed;
    }

    //TODO: Translate velocity to rotation

    // Update position
    glm::vec3 pos = GetPosition();
    pos.x += poVelocity.x;
    pos.z += poVelocity.y;
    SetPosition(pos);

    // Prevents the entity from speeding too far
    if (poVelocity.x > 0.001 && poVelocity.y > 0.001)
    {
        poVelocity *= 0.9;
    }
}

/*
 * Update logic, first let the handler update the behaviour, then update position according to the calculated steering force.
 */
void cEntity::Update()
{
    poSteeringForce = glm::vec2(0, 0);
    for (auto &cBehaviourHandler : paBehaviourHandlers)
    {
        // Runs JavaScript which calculates a steering force and appends it to the current force.
        cBehaviourHandler->Update(this);
    }

    UpdatePosition();
}

