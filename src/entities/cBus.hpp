#pragma once

#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>
#include <vulkan/entities/cEntityGroup.hpp>
#include <entities/IPassengerHolder.hpp>

const float C_EMPTY_FLOAT = 0.0f;
const int C_UNDEFINED = -1;

// Steering & Acceleration values
const float C_MAX_SPEED = 40.0f;
const float C_MIN_SPEED = -15.0f;
const float C_MAX_STEERING = 15.0f;
const float C_MIN_STEERING = -15.0f;

// Default collision values
const float C_COLL_X1 = -1.5f;
const float C_COLL_Z1 = -7.0f;
const float C_COLL_X2 = 1.6f;
const float C_COLL_Z2 = 2.7f;

enum class Direction
{
    Left, Right
};

class cBus : public cBaseObject, IPassengerHolder
{
public:
    float pfMaxSpeed = C_MAX_SPEED;
    float pfMinSpeed = C_MIN_SPEED;
    float pfCurrentSpeed = C_EMPTY_FLOAT;
    float pfAccelerationModifier = C_EMPTY_FLOAT;
    float pfSteeringModifier = C_EMPTY_FLOAT;
    float pfMaxSteeringModifier = C_MAX_STEERING;
    float pfMinSteeringModifier = C_MIN_STEERING;
    int piPingTimeout = C_UNDEFINED;
    int piBusId = C_UNDEFINED;

    cEntityGroup *entityGroup;
    cAudioHandler *ppAudioHandler;
    int piEngineChannel;
    int piEngineAccelChannel;
    int piEngineDecelChannel;
    int piIdleChannel;

    cBus(cAudioHandler *pAudioHandler, cMesh *mesh) : cBaseObject(mesh,
                                                                  cCollider::RectangleCollider(C_COLL_X1, C_COLL_Z1,
                                                                                               C_COLL_X2, C_COLL_Z2),
                                                                  false)
    {
        ppAudioHandler = pAudioHandler;

        ppAudioHandler->LoadSound("resources/audio/VOLUME_engine.wav", true, true, false);
        piEngineChannel = ppAudioHandler->PlaySound("resources/audio/VOLUME_engine.wav", GetPosition(), 0.1f);
        ppAudioHandler->SetPaused(piEngineChannel, true);

        ppAudioHandler->LoadSound("resources/audio/engine-idle.wav", true, true, false);
        piIdleChannel = ppAudioHandler->PlaySound("resources/audio/engine-idle.wav", GetPosition(), 0.8f);
        ppAudioHandler->SetPaused(piIdleChannel, true);
    }

    float CalculateAcceleration();

    float CalculateDeceleration();

    void Move();

    void Steer(Direction direction);

    void Accelerate();

    void Decelerate();

    void IdleAcceleration();

    void IdleSteering();

    glm::vec3 GetDoorPosition();

    bool AddPassenger(IPassenger *passenger) override;

    bool RemovePassenger(IPassenger *passenger) override;

};

void cBus::Move()
{
    glm::vec3 oDirection(sin(glm::radians(GetRotation().y)), 0, cos(glm::radians(GetRotation().y)));
    bool moveCollision = !SetPosition(GetPosition() - (oDirection * (pfCurrentSpeed / 100)));
    bool steerCollision = false;

    // If bus is moving, rotate steering wheel and play engine sound. If not play engine idle sound and don't use the steering wheel.
    if (pfCurrentSpeed != 0)
    {
        if (pfSteeringModifier > 0)
        {
            steerCollision = !this->RotateLeft(pfSteeringModifier / 10);
        } else
        {
            steerCollision = !this->RotateRight(pfSteeringModifier * -1 / 10);
        }
        ppAudioHandler->SetPaused(piIdleChannel, true);
        ppAudioHandler->SetChannelPosition(piEngineChannel, GetPosition());
        ppAudioHandler->SetPaused(piEngineChannel, false);
    } else
    {
        ppAudioHandler->SetPaused(piEngineChannel, true);
        ppAudioHandler->SetChannelPosition(piIdleChannel, GetPosition());
        ppAudioHandler->SetPaused(piIdleChannel, false);
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
    if (pfCurrentSpeed > 0.5f)
    {
        pfCurrentSpeed *= 0.997f;
    }
    if (pfCurrentSpeed < -0.5f)
    {
        pfCurrentSpeed *= 0.997f;
    }
    if (pfCurrentSpeed < 0.5f && pfCurrentSpeed > -0.5f)
    {
        pfCurrentSpeed = 0.0f;
    }
}

void cBus::Steer(Direction direction)
{
    // Block steering while stopped
    if (pfCurrentSpeed == 0) return;

    if (direction == Direction::Left)
    {
        // If the steering modifier is still set to right (lower than 0), reset the modifier to 0.
        if (pfSteeringModifier < 0)
        {
            pfSteeringModifier = 0;
        }
        // Only higher the modifier if the max value hasn't been reached.
        if (pfSteeringModifier < pfMaxSteeringModifier)
        {
            pfSteeringModifier += 0.2f;
        }
    }
    if (direction == Direction::Right)
    {
        // If the steering modifier is still set to left (higher than 0), reset the modifier to 0.
        if (pfSteeringModifier > 0)
        {
            pfSteeringModifier = 0;
        }
        // Only higher the modifier if the min value hasn't been reached.
        if (pfSteeringModifier > pfMinSteeringModifier)
        {
            pfSteeringModifier -= 0.2f;
        }
    }
}

void cBus::IdleSteering()
{
    // No hands touching the steering wheel, it will slowly rotate back to 0.
    pfSteeringModifier *= 0.985f;
}

/*
 * Function  to make the vehicle accelerate slower if it's going faster.
 */
float cBus::CalculateAcceleration()
{
    for (float fVal = 0.1f; fVal <= 1.0f; fVal += 0.1f)
    {
        if (pfMaxSpeed * fVal >= pfCurrentSpeed)
        {
            pfAccelerationModifier = 1.1f - fVal;
            ppAudioHandler->SetChannelVolume(piEngineChannel, 1.1f - pfAccelerationModifier);
            return pfAccelerationModifier;
        }
    }
    return 0.0;
}

/*
 * Function to make the vehicle decelerate slower if it's going faster.
 */
float cBus::CalculateDeceleration()
{
    for (float fVal = 0.7f; fVal >= 0.1f; fVal -= 0.1f)
    {
        if (pfMinSpeed * fVal <= pfCurrentSpeed)
        {
            pfAccelerationModifier = 1.1f - fVal;
            return pfAccelerationModifier;
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

bool cBus::AddPassenger(IPassenger *passenger)
{
    return false;
}

bool cBus::RemovePassenger(IPassenger *passenger)
{
    return false;
}
