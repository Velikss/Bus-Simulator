#pragma once

#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>

class cBus : public cBaseObject
{
public:
    float pfMaxSpeed = 40;
    float pfMinSpeed = -15;
    float pfCurrentSpeed;
    float pfAccelerationModifier;
    float pfSteeringModifier;
    float pfMaxSteeringModifier = 15;
    float pfMinSteeringModifier = -15;

    int piPingTimeout;
    int piBusId;

    cBus(cMesh* mesh) : cBaseObject(mesh, cCollider::RectangleCollider(-1.5f, -7.0f, 1.6f, 2.7f), false)
    {
    }

    float CalculateAcceleration();

    float CalculateDeceleration();

    void Move();

    void Steer(std::string sDirection);

    void Accelerate();

    void Decelerate();

    void IdleAcceleration();

    void IdleSteering();

    glm::vec3 GetDoorPosition();

};

void cBus::Move()
{
    glm::vec3 oDirection(sin(glm::radians(GetRotation().y)), 0, cos(glm::radians(GetRotation().y)));
    bool moveCollision = !SetPosition(GetPosition() - (oDirection * (pfCurrentSpeed / 100)));

    bool steerCollision = false;
    if (pfCurrentSpeed != 0)
    {
        if (pfSteeringModifier > 0)
        {
            steerCollision = !this->RotateLeft(pfSteeringModifier / 10);
        }
        else
        {
            steerCollision = !this->RotateRight(pfSteeringModifier * -1 / 10);
        }
    }

    if (steerCollision || moveCollision)
    {
        pfAccelerationModifier = 0;
        pfSteeringModifier = 0;
        pfCurrentSpeed = 0;
    }
}

void cBus::Accelerate()
{
    if (pfCurrentSpeed < pfMaxSpeed)
    {
        pfCurrentSpeed += CalculateAcceleration() / 2;
    }
}

void cBus::Decelerate()
{
    if (pfCurrentSpeed > pfMinSpeed)
    {
        pfCurrentSpeed -= CalculateDeceleration() / 2;
    }
}

void cBus::IdleAcceleration()
{
    if (pfCurrentSpeed > 0.5)
    {
        pfCurrentSpeed *= 0.997;
    }
    if (pfCurrentSpeed < -0.5)
    {
        pfCurrentSpeed *= 0.997;
    }
    if (pfCurrentSpeed < 0.5 && pfCurrentSpeed > -0.5)
    {
        pfCurrentSpeed = 0;
    }
}

void cBus::Steer(std::string sDirection)
{
    // Block steering while stopped
    if (pfCurrentSpeed == 0) return;

    if (sDirection == "left")
    {
        // If the steering modifier is still set to right (lower than 0), reset the modifier to 0.
        if (pfSteeringModifier < 0)
        {
            pfSteeringModifier = 0;
        }
        // Only higher the modifier if the max value hasn't been reached.
        if (pfSteeringModifier < pfMaxSteeringModifier)
        {
            pfSteeringModifier += 0.2;
        }
    }
    if (sDirection == "right")
    {
        // If the steering modifier is still set to left (higher than 0), reset the modifier to 0.
        if (pfSteeringModifier > 0)
        {
            pfSteeringModifier = 0;
        }
        // Only higher the modifier if the min value hasn't been reached.
        if (pfSteeringModifier > pfMinSteeringModifier)
        {
            pfSteeringModifier -= 0.2;
        }
    }
}

void cBus::IdleSteering()
{
    // No hands touching the steering wheel, it will slowly rotate back to 0.
    pfSteeringModifier *= 0.985;
}

/*
 * Function  to make the vehicle accelerate slower if it's going faster.
 */
float cBus::CalculateAcceleration()
{
    for (float fVal = 0.1; fVal <= 1.0; fVal += 0.1)
    {
        if (pfMaxSpeed * fVal >= pfCurrentSpeed)
        {
            return pfAccelerationModifier = 1.1 - fVal;
        }
    }
    return 0.0;
}

/*
 * Function  to make the vehicle decelerate slower if it's going faster.
 */
float cBus::CalculateDeceleration()
{
    for (float fVal = 0.7; fVal >= 0.1; fVal -= 0.1)
    {
        if (pfMinSpeed * fVal <= pfCurrentSpeed)
        {
            return pfAccelerationModifier = 1.1 - fVal;
        }
    }
    return 0.0;
}

glm::vec3 cBus::GetDoorPosition()
{
    // Not relative to the scale, only works with bus scale = 0.8
    glm::vec3 direction(sin(glm::radians(GetRotation().y)), 0,
                        cos(glm::radians(GetRotation().y)));
    glm::vec3 doorPos = GetPosition();
    doorPos -= (direction * 3.5f);
    return doorPos;
}
