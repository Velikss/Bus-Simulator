#pragma once

#include <pch.hpp>
#include <objects/BaseObject.hpp>

class cEntityInterface : public BaseObject
{
public:
    cEntityInterface(Mesh *mesh) : BaseObject(mesh)
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

};

class cEntityGroupInterface : public cEntityInterface
{
public:
    cEntityGroupInterface() : cEntityInterface(nullptr)
    {

    }

    virtual void ReturnEntities(std::vector<cEntityInterface *> **entities) = 0;

};

