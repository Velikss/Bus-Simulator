#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>

class iCommandBufferRecorder
{
public:
    virtual ~iCommandBufferRecorder();

    virtual void Setup(uint uiIndex) = 0;
    virtual void RecordCommands(VkCommandBuffer& oCommandBuffer, uint uiIndex) = 0;
};

iCommandBufferRecorder::~iCommandBufferRecorder()
{
}
