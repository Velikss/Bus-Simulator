#pragma once

#include <pch.hpp>
#include <vulkan/LogicalDevice.hpp>

class cRenderPass
{
protected:
    VkRenderPass poRenderPass = VK_NULL_HANDLE;

    cLogicalDevice* ppLogicalDevice;

public:
    cRenderPass(cLogicalDevice* pLogicalDevice);
    ~cRenderPass();

    VkRenderPass& GetRenderPass();
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

VkRenderPass& cRenderPass::GetRenderPass()
{
    assert(poRenderPass != VK_NULL_HANDLE);

    return poRenderPass;
}
