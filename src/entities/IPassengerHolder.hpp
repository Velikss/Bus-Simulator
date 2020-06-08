#pragma once

#include <pch.hpp>
#include <entities/IPassenger.hpp>

class IPassengerHolder
{
public:
    cEntityGroup* poEntityGroup = nullptr;

    virtual bool AddPassenger(IPassenger* pPassenger) = 0;
    virtual bool RemovePassenger(IPassenger* pPassenger) = 0;
};