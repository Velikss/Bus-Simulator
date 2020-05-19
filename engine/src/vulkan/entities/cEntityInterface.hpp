#pragma once

#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>

class cEntityInterface : public cBaseObject
{
public:
    cEntityInterface(cMesh *mesh) : cBaseObject(mesh, false)
    {

    }

    virtual void SetMass(float fMass)
    {

    }

    virtual float GetMass()
    {

    }

    virtual void SetMaxSpeed(float fSpeed)
    {

    }

    virtual float GetMaxSpeed()
    {

    }

    virtual void SetHeading(glm::vec2 oHeading)
    {

    }

    virtual glm::vec2 GetHeading()
    {

    }

    virtual void SetVelocity(glm::vec2 oVelocity)
    {

    }

    virtual glm::vec2 GetVelocity()
    {

    }

    virtual void SetSteeringForce(glm::vec2 oSteeringForce)
    {

    }

    virtual glm::vec2 GetSteeringForce()
    {

    }

    virtual void SetTarget(glm::vec3 oTarget)
    {

    }

    virtual glm::vec3 GetTarget()
    {

    }

    virtual void AppendSteeringForce(glm::vec2 oSteeringForce)
    {

    }

};

class cEntityGroupInterface : public cEntityInterface
{
public:
    cEntityGroupInterface() : cEntityInterface(nullptr)
    {

    }

    virtual void ReturnEntities(std::vector<cEntityInterface *> **entities) = 0;

};

