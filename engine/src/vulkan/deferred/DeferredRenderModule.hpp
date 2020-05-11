#pragma once

#include <pch.hpp>
#include <vulkan/module/RenderModule.hpp>
#include <vulkan/deferred/DeferredRenderPass.hpp>
#include <vulkan/deferred/DeferredPipeline.hpp>
#include <vulkan/deferred/DeferredUniformHandler.hpp>

class cMRTRenderModule : public cRenderModule
{
public:
    cMRTRenderModule(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain);

protected:
    void CreateUniformHandler() override;
    void CreateRenderPass() override;
    void CreatePipeline() override;
};

cMRTRenderModule::cMRTRenderModule(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain)
        : cRenderModule(pLogicalDevice, pSwapChain)
{
    Init();
}

void cMRTRenderModule::CreateUniformHandler()
{
    // Create the uniform handler. This is responsible for the uniform variables
    ppUniformHandler = new cMRTUniformHandler(ppLogicalDevice, ppSwapChain);
}

void cMRTRenderModule::CreateRenderPass()
{
    // Create the render pass. This holds information about the frames we want to render.
    // will probably be moved somewhere else later
    ppRenderPass = new cMRTRenderPass(ppLogicalDevice, ppSwapChain);
}

void cMRTRenderModule::CreatePipeline()
{
    // Create the graphics pipeline. Handles the shaders and fixed-function operations for the graphics pipeline.
    ppRenderPipeline = new cMRTPipeline(ppSwapChain, ppLogicalDevice, ppRenderPass, ppUniformHandler);
}
