#pragma once

#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>

class cEntityBaseInterface : public cBaseObject
{
public:
    cEntityBaseInterface(cMesh *mesh) : cBaseObject(mesh, false)
    {

    }
};

class cEntityInterface : public cEntityBaseInterface
{
public:
    cEntityInterface(cMesh *mesh) : cEntityBaseInterface(mesh)
    {

    }

    virtual void SetMass(float fMass) = 0;

    virtual float GetMass() = 0;

    virtual void SetMaxSpeed(float fSpeed) = 0;

    virtual float GetMaxSpeed() = 0;

    virtual void SetHeading(glm::vec2 oHeading) = 0;

    virtual glm::vec2 GetHeading() = 0;

    virtual void SetVelocity(glm::vec2 oVelocity) = 0;

    virtual glm::vec2 GetVelocity() = 0;

    virtual void SetTarget(glm::vec3 oTarget) = 0;

    virtual glm::vec3 GetTarget() = 0;

    virtual void SetSteeringForce(glm::vec2 oSteeringForce) = 0;

    virtual glm::vec2 GetSteeringForce() = 0;

    virtual void AppendSteeringForce(glm::vec2 oSteeringForce) = 0;

};

class cEntityGroupInterface : public cEntityBaseInterface
{
public:
    cEntityGroupInterface() : cEntityBaseInterface(nullptr)
    {

    }

    virtual void ReturnEntities(std::vector<cEntityInterface *> **entities) = 0;
};

