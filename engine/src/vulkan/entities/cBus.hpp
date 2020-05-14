#pragma once

#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>

class cBus : public cBaseObject
{
public:
    float pfMaxSpeed;
    float pfMinSpeed;
    float pfCurrentSpeed;
    float pfAcceleration;

    cBus(cMesh * mesh) : cBaseObject(mesh)
    {
        pfMaxSpeed = 40;
        pfMinSpeed = -15;
        pfCurrentSpeed = 0;
        pfAcceleration = 0;
    }

    float CalculateAcceleration();
    float CalculateBrakeForce();
    void Move();
    void Throttle();
    void Brake();
    void Idle();
    void MoveLeft(float fAngleDiff) override;
    void MoveRight(float fAngleDiff) override ;
};

void cBus::Move() {
    glm::vec3 direction(sin(glm::radians(poRotation.y)), 0, cos(glm::radians(poRotation.y)));
    poPosition -= (direction * (pfCurrentSpeed / 100));
}

void cBus::Throttle() {
    if (pfCurrentSpeed < pfMaxSpeed)
        pfCurrentSpeed+= CalculateAcceleration() / 2;
}

void cBus::Brake() {
    if (pfCurrentSpeed > pfMinSpeed)
        pfCurrentSpeed-= CalculateBrakeForce() / 2;
}

void cBus::Idle() {
    if(pfCurrentSpeed > 0.1)
    {
        pfCurrentSpeed *= 0.995;
    }
    if(pfCurrentSpeed < -0.1)
    {
        pfCurrentSpeed *= 0.995;
    }
    if(pfCurrentSpeed < 0.8 && pfCurrentSpeed > -0.8)
        pfCurrentSpeed = 0;
}

void cBus::MoveLeft(float fAngleDiff)
{
    if(pfCurrentSpeed == 0) return;

    if (poRotation.y >= 360.0f)
        poRotation.y = 0;
    poRotation.y += fAngleDiff;
}

void cBus::MoveRight(float fAngleDiff)
{
    if(pfCurrentSpeed == 0) return;

    if (poRotation.y < 0.0f)
        poRotation.y = 360.0f;
    poRotation.y -= fAngleDiff;
}

float cBus::CalculateAcceleration() {
    if((pfMaxSpeed * 0.1) >= pfCurrentSpeed)
        return pfAcceleration = 1.0;
    if((pfMaxSpeed * 0.2) >= pfCurrentSpeed)
        return pfAcceleration = 0.9;
    if((pfMaxSpeed * 0.3) >= pfCurrentSpeed)
        return pfAcceleration = 0.8;
    if((pfMaxSpeed * 0.4) >= pfCurrentSpeed)
        return pfAcceleration = 0.7;
    if((pfMaxSpeed * 0.5) >= pfCurrentSpeed)
        return pfAcceleration = 0.6;
    if((pfMaxSpeed * 0.6) >= pfCurrentSpeed)
        return pfAcceleration = 0.5;
    if((pfMaxSpeed * 0.7) >= pfCurrentSpeed)
        return pfAcceleration = 0.4;
    if((pfMaxSpeed * 0.8) >= pfCurrentSpeed)
        return pfAcceleration = 0.3;
    if((pfMaxSpeed * 0.9) >= pfCurrentSpeed)
        return pfAcceleration = 0.2;
    if((pfMaxSpeed * 1.0) >= pfCurrentSpeed)
        return pfAcceleration = 0.1;
}

float cBus::CalculateBrakeForce() {
    if((pfMinSpeed * 0.7) <= pfCurrentSpeed)
        return pfAcceleration = 0.4;
    if((pfMinSpeed * 0.6) <= pfCurrentSpeed)
        return pfAcceleration = 0.5;
    if((pfMinSpeed * 0.5) <= pfCurrentSpeed)
        return pfAcceleration = 0.6;
    if((pfMinSpeed * 0.4) <= pfCurrentSpeed)
        return pfAcceleration = 0.7;
    if((pfMinSpeed * 0.3) <= pfCurrentSpeed)
        return pfAcceleration = 0.8;
    if((pfMinSpeed * 0.2) <= pfCurrentSpeed)
        return pfAcceleration = 0.9;
    if((pfMinSpeed * 0.1) <= pfCurrentSpeed)
        return pfAcceleration = 1.0;
}
