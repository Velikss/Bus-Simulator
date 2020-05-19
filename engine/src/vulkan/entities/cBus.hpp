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

    cBus(cMesh * mesh) : cBaseObject(mesh)
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

};

void cBus::Move() {
    glm::vec3 oDirection(sin(glm::radians(poRotation.y)), 0, cos(glm::radians(poRotation.y)));
    poPosition -= (oDirection * (pfCurrentSpeed / 100));
    if(pfCurrentSpeed !=0)
        pfSteeringModifier > 0 ? this->RotateLeft(pfSteeringModifier / 10) : this->RotateRight(pfSteeringModifier * -1 / 10);
}

void cBus::Accelerate() {
    if (pfCurrentSpeed < pfMaxSpeed)
        pfCurrentSpeed+= CalculateAcceleration() / 2;
}

void cBus::Decelerate() {
    if (pfCurrentSpeed > pfMinSpeed)
        pfCurrentSpeed-= CalculateDeceleration() / 2;
}

void cBus::IdleAcceleration()
{
    if(pfCurrentSpeed > 0.5)
    {
        pfCurrentSpeed *= 0.997;
    }
    if(pfCurrentSpeed < -0.5)
    {
        pfCurrentSpeed *= 0.997;
    }
    if(pfCurrentSpeed < 0.5 && pfCurrentSpeed > -0.5)
        pfCurrentSpeed = 0;
}

void cBus::Steer(std::string sDirection) {
    // Block steering while stopped
    if(pfCurrentSpeed == 0) return;

    if(sDirection == "left")
    {
        // If the steering modifier is still set to right (lower than 0), reset the modifier to 0.
        if(pfSteeringModifier < 0)
            pfSteeringModifier = 0;
        // Only higher the modifier if the max value hasn't been reached.
        if (pfSteeringModifier < pfMaxSteeringModifier)
            pfSteeringModifier += 0.2;
    }
    if (sDirection == "right")
    {
        // If the steering modifier is still set to left (higher than 0), reset the modifier to 0.
        if(pfSteeringModifier > 0)
            pfSteeringModifier = 0;
        // Only higher the modifier if the min value hasn't been reached.
        if (pfSteeringModifier > pfMinSteeringModifier)
            pfSteeringModifier -= 0.2;
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
float cBus::CalculateAcceleration() {
    if((pfMaxSpeed * 0.1) >= pfCurrentSpeed)
        return pfAccelerationModifier = 1.0;
    if((pfMaxSpeed * 0.2) >= pfCurrentSpeed)
        return pfAccelerationModifier = 0.9;
    if((pfMaxSpeed * 0.3) >= pfCurrentSpeed)
        return pfAccelerationModifier = 0.8;
    if((pfMaxSpeed * 0.4) >= pfCurrentSpeed)
        return pfAccelerationModifier = 0.7;
    if((pfMaxSpeed * 0.5) >= pfCurrentSpeed)
        return pfAccelerationModifier = 0.6;
    if((pfMaxSpeed * 0.6) >= pfCurrentSpeed)
        return pfAccelerationModifier = 0.5;
    if((pfMaxSpeed * 0.7) >= pfCurrentSpeed)
        return pfAccelerationModifier = 0.4;
    if((pfMaxSpeed * 0.8) >= pfCurrentSpeed)
        return pfAccelerationModifier = 0.3;
    if((pfMaxSpeed * 0.9) >= pfCurrentSpeed)
        return pfAccelerationModifier = 0.2;
    if((pfMaxSpeed * 1.0) >= pfCurrentSpeed)
        return pfAccelerationModifier = 0.1;
}
/*
 * Function  to make the vehicle decelerate slower if it's going faster.
 */
float cBus::CalculateDeceleration() {
    if((pfMinSpeed * 0.7) <= pfCurrentSpeed)
        return pfAccelerationModifier = 0.4;
    if((pfMinSpeed * 0.6) <= pfCurrentSpeed)
        return pfAccelerationModifier = 0.5;
    if((pfMinSpeed * 0.5) <= pfCurrentSpeed)
        return pfAccelerationModifier = 0.6;
    if((pfMinSpeed * 0.4) <= pfCurrentSpeed)
        return pfAccelerationModifier = 0.7;
    if((pfMinSpeed * 0.3) <= pfCurrentSpeed)
        return pfAccelerationModifier = 0.8;
    if((pfMinSpeed * 0.2) <= pfCurrentSpeed)
        return pfAccelerationModifier = 0.9;
    if((pfMinSpeed * 0.1) <= pfCurrentSpeed)
        return pfAccelerationModifier = 1.0;
}
