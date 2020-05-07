#pragma once

#include <pch.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/SwapChain.hpp>
#include <vulkan/renderpass/RenderPass.hpp>
#include <vulkan/uniform/UniformHandler.hpp>

class cRenderPipeline
{
protected:
    VkPipelineLayout poPipelineLayout = VK_NULL_HANDLE;
    VkPipeline poPipeline = VK_NULL_HANDLE;

    cLogicalDevice* ppLogicalDevice;

public:
    virtual ~cRenderPipeline();

    VkPipelineLayout& GetLayout();
    VkPipeline& GetPipeline();

protected:
    void Init(cSwapChain* pSwapChain,
              cLogicalDevice* pLogicalDevice,
              cRenderPass* pRenderPass,
              iUniformHandler* pUniformHandler);

    virtual void CreatePipelineLayout(cSwapChain* pSwapChain,
                                      cLogicalDevice* pLogicalDevice,
                                      cRenderPass* pRenderPass,
                                      iUniformHandler* pUniformHandler) = 0;
    virtual void CreatePipeline(cSwapChain* pSwapChain,
                                cLogicalDevice* pLogicalDevice,
                                cRenderPass* pRenderPass,
                                iUniformHandler* pUniformHandler) = 0;
};

void cRenderPipeline::Init(cSwapChain* pSwapChain,
                           cLogicalDevice* pLogicalDevice,
                           cRenderPass* pRenderPass,
                           iUniformHandler* pUniformHandler)
{
    assert(pSwapChain != nullptr);
    assert(pLogicalDevice != nullptr);
    assert(pRenderPass != nullptr);
    assert(pUniformHandler != nullptr);

    ppLogicalDevice = pLogicalDevice;

    CreatePipelineLayout(pSwapChain, pLogicalDevice, pRenderPass, pUniformHandler);
    assert(poPipelineLayout != VK_NULL_HANDLE); // pipeline layout should be created

    CreatePipeline(pSwapChain, pLogicalDevice, pRenderPass, pUniformHandler);
    assert(poPipeline != VK_NULL_HANDLE); // pipeline should be created
}

cRenderPipeline::~cRenderPipeline()
{
    if (poPipeline != VK_NULL_HANDLE)
    {
        ppLogicalDevice->DestroyPipeline(poPipeline, nullptr);
    }
    if (poPipelineLayout != VK_NULL_HANDLE)
    {
        ppLogicalDevice->DestroyPipelineLayout(poPipelineLayout, nullptr);
    }
}

VkPipelineLayout& cRenderPipeline::GetLayout()
{
    assert(poPipelineLayout != VK_NULL_HANDLE); // render pipeline needs to be initialized first

    return poPipelineLayout;
}

VkPipeline& cRenderPipeline::GetPipeline()
{
    assert(poPipeline != VK_NULL_HANDLE); // render pipeline needs to be initialized first

    return poPipeline;
}
