#pragma once

#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>

class IBaseEntity : public cBaseObject
{
public:
    IBaseEntity(cMesh* mesh) : cBaseObject(mesh, nullptr, false)
    {

    }
};

class IEntity : public IBaseEntity
{
public:
    IEntity(cMesh* mesh) : IBaseEntity(mesh)
    {

    }

    virtual void SetMaxSpeed(float fSpeed) = 0;

    virtual float GetMaxSpeed() = 0;

    virtual void SetVelocity(glm::vec2 oVelocity) = 0;

    virtual glm::vec2 GetVelocity() = 0;

    virtual void SetTarget(glm::vec3 oTarget) = 0;

    virtual glm::vec3 GetTarget() = 0;

    virtual void SetSteeringForce(glm::vec2 oSteeringForce) = 0;

    virtual glm::vec2 GetSteeringForce() = 0;

    virtual void AppendSteeringForce(glm::vec2 oSteeringForce) = 0;

};

class IEntityGroup : public IBaseEntity
{
public:
    IEntityGroup() : IBaseEntity(nullptr)
    {

    }

    virtual void GetEntityList(std::vector<IEntity*>** entities) = 0;

    virtual std::vector<IBaseEntity*>* GetEntities() = 0;
};

