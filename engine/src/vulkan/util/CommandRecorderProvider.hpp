#pragma once

#include <pch.hpp>
#include <vulkan/command/CommandBufferRecorder.hpp>

class iCommandRecorderProvider
{
public:
    virtual iCommandBufferRecorder* GetCommandRecorder() = 0;
};
