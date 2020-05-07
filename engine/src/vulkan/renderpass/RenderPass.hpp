#pragma once

#include <pch.hpp>
#include <vulkan/LogicalDevice.hpp>

class cRenderPass
{
protected:
    cLogicalDevice* ppLogicalDevice;

public:
    VkRenderPass poRenderPass = VK_NULL_HANDLE;

    cRenderPass(cLogicalDevice* pLogicalDevice);
    ~cRenderPass();
};

cRenderPass::cRenderPass(cLogicalDevice* pLogicalDevice)
{
    assert(pLogicalDevice != nullptr);

    ppLogicalDevice = pLogicalDevice;
}

cRenderPass::~cRenderPass()
{
    if (poRenderPass != VK_NULL_HANDLE)
    {
        vkDestroyRenderPass(ppLogicalDevice->GetDevice(), poRenderPass, nullptr);
    }
}
