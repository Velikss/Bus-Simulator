#pragma once

#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>

class cEntityInterface : public cBaseObject
{
public:
    cEntityInterface(cMesh *mesh) : cBaseObject(mesh, false)
    {

    }

    virtual void ReturnMass(float *mass)
    {

    }

    virtual void ReturnMaxSpeed(float * speed)
    {

    }

    virtual void ReturnHeading(glm::vec2 *heading)
    {

    }

    virtual void ReturnVelocity(glm::vec2 *velocity)
    {

    }

    virtual void SetVelocity(glm::vec2 *velocity)
    {

    }

    virtual void SetHeading(glm::vec2 *heading)
    {

    }

    virtual void AppendSteeringForce(glm::vec2 *SteeringForce)
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

