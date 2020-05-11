#pragma once

#include <pch.hpp>
#include <vulkan/module/RenderModule.hpp>
#include <vulkan/GraphicsRenderPass.hpp>
#include <vulkan/uniform/LightsUniformHandler.hpp>

class cLightsRenderModule : public cRenderModule
{
public:
    cLightsRenderModule(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain);

protected:
    void CreateUniformHandler() override;
    void CreateRenderPass() override;
    void CreatePipeline() override;
};

cLightsRenderModule::cLightsRenderModule(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain)
        : cRenderModule(pLogicalDevice, pSwapChain)
{
    Init();
}

void cLightsRenderModule::CreateUniformHandler()
{
    // Create the uniform handler. This is responsible for the uniform variables
    ppUniformHandler = new cLightsUniformHandler(ppLogicalDevice, ppSwapChain);
}

void cLightsRenderModule::CreateRenderPass()
{
    // Create the render pass. This holds information about the frames we want to render.
    // will probably be moved somewhere else later
    ppRenderPass = new cLightsRenderPass(ppLogicalDevice, ppSwapChain);
}

void cLightsRenderModule::CreatePipeline()
{
    // Create the graphics pipeline. Handles the shaders and fixed-function operations for the graphics pipeline.
    ppRenderPipeline = new cLightsPipeline(ppSwapChain, ppLogicalDevice, ppRenderPass, ppUniformHandler);
}
