#pragma once

#include <pch.hpp>
#include <vulkan/command/CommandBuffer.hpp>

class iCommandBufferHolder
{
public:
    virtual cCommandBuffer** GetCommandBuffers() = 0;
};
