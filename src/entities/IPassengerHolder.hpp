#pragma once

#include <pch.hpp>
#include <entities/IPassenger.hpp>

class IPassengerHolder
{
public:
    virtual bool AddPassenger(IPassenger* pPassenger) = 0;
    virtual bool RemovePassenger(IPassenger* pPassenger) = 0;
};