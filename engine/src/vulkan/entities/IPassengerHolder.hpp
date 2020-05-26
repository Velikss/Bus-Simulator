#pragma once

#include <pch.hpp>
#include <vulkan/entities/IPassenger.hpp>

class IPassengerHolder
{
public:
    virtual bool AddPassenger(IPassenger* passenger) = 0;
    virtual bool RemovePassenger(IPassenger* passenger) = 0;
};