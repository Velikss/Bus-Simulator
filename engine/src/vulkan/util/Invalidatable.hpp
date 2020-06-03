#pragma once

#include <pch.hpp>

class cInvalidatable
{
private:
    bool pbInvalidated = true;

public:
    virtual bool Invalidated();
    virtual void Validate();
    virtual void Invalidate();
};

bool cInvalidatable::Invalidated()
{
    return pbInvalidated;
}

void cInvalidatable::Validate()
{
    pbInvalidated = false;
}

void cInvalidatable::Invalidate()
{
    pbInvalidated = true;
}
