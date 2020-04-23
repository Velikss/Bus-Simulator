#pragma once

#include <pch.hpp>
#include <objects/BaseObject.hpp>

class cEntityInterface : public BaseObject
{
public:
    cEntityInterface(Mesh *mesh) : BaseObject(mesh)
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

