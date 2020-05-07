#pragma once

#include <pch.hpp>
#include <vulkan/module/RenderModule.hpp>
#include <vulkan/uniform/GraphicsUniformHandler.hpp>
#include <vulkan/GraphicsRenderPass.hpp>

class cGraphicsRenderModule : public cRenderModule
{
public:
    cGraphicsRenderModule(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain);

protected:
    void CreateUniformHandler() override;
    void CreateRenderPass() override;
    void CreatePipeline() override;
};

cGraphicsRenderModule::cGraphicsRenderModule(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain)
        : cRenderModule(pLogicalDevice, pSwapChain)
{
    Init();
}

void cGraphicsRenderModule::CreateUniformHandler()
{
    // Create the uniform handler. This is responsible for the uniform variables
    ppUniformHandler = new cGraphicsUniformHandler(ppLogicalDevice, ppSwapChain);
}

void cGraphicsRenderModule::CreateRenderPass()
{
    // Create the render pass. This holds information about the frames we want to render.
    // will probably be moved somewhere else later
    ppRenderPass = new cGraphicsRenderPass(ppLogicalDevice, ppSwapChain);
}

void cGraphicsRenderModule::CreatePipeline()
{
    // Create the graphics pipeline. Handles the shaders and fixed-function operations for the graphics pipeline.
    ppRenderPipeline = new cGraphicsPipeline(ppSwapChain, ppLogicalDevice, ppRenderPass, ppUniformHandler);
}
