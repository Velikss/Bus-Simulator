#pragma once

#include <pch.hpp>

class iTickTask
{
public:
    virtual ~iTickTask();
    virtual void Tick() = 0;
};

iTickTask::~iTickTask()
{
}
