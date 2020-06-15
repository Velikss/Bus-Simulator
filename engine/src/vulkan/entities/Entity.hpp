#pragma once

#include <pch.hpp>
#include <vulkan/entities/BehaviourHandler.hpp>
#include <vulkan/entities/BaseEntity.hpp>

class cEntity : public IEntity
{
private:
    std::vector<cBehaviourHandler*> paBehaviourHandlers;
    glm::vec3 poTarget;
    glm::vec2 poVelocity;
    glm::vec2 poSteeringForce;
    float pfMaxSpeed;
public:
    bool pbRotateOnMove;
    cEntity(cMesh *mesh, bool bRotateOnMove = false) : IEntity(mesh)
    {
        poVelocity = glm::vec2(0, 0);
        poSteeringForce = glm::vec2(0, 0);
        pfMaxSpeed = 0.1;
        pbRotateOnMove = bRotateOnMove;
    }

    void AddBehaviour(cBehaviourHandler*& poBehaviour);

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

void cEntity::AddBehaviour(cBehaviourHandler*& poBehaviour)
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

void cEntity::SetTarget(glm::vec3 oTarget)
{
    poTarget = oTarget;
}

glm::vec3 cEntity::GetTarget()
{
    return poTarget;
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

void cEntity::UpdatePosition()
{
    if (poSteeringForce != glm::vec2(0, 0))
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
        if(pbRotateOnMove)
        {
            float angle = glm::degrees(atan2(poVelocity.y, poVelocity.x));
            if (angle > 0)
            {
                this->SetRotation(glm::vec3(0.0f, 360 - angle, 0.0f));
            }
            else
            {
                this->SetRotation(glm::vec3(0.0f, angle * -1, 0.0f));
            }
        }
     }

    if (poVelocity.x > 0.001 && poVelocity.y > 0.001)
    {
        poVelocity *= 0.9;
    }
}

void cEntity::Update()
{
    if (!paBehaviourHandlers.empty())
    {
        poSteeringForce = glm::vec2(0, 0);
        for (auto& cBehaviourHandler : paBehaviourHandlers)
        {
            // Runs JavaScript which calculates a steering force and appends it to the current force.
            cBehaviourHandler->Update(this);
        }

        UpdatePosition();
    }
}

