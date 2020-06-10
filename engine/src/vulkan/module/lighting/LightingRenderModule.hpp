#pragma once

#include <pch.hpp>
#include <vulkan/module/RenderModule.hpp>
#include <vulkan/module/lighting/LightingRenderPass.hpp>
#include <vulkan/module/lighting/LightingUniformHandler.hpp>

class cLightingRenderModule : public cRenderModule
{
public:
    cLightingRenderModule(cLogicalDevice* pLogicalDevice,
                          cSwapChain* pSwapChain,
                          std::vector<string>& aShaders);
    ~cLightingRenderModule() override;

protected:
    void CreateUniformHandler() override;
    void CreateRenderPass() override;
    void CreatePipeline(std::vector<string>& aShaders) override;
};

cLightingRenderModule::cLightingRenderModule(cLogicalDevice* pLogicalDevice,
                                             cSwapChain* pSwapChain,
                                             std::vector<string>& aShaders)
        : cRenderModule(pLogicalDevice, pSwapChain, aShaders)
{
    Init();

    cViewportQuadGeometry::Init(pLogicalDevice);
}

cLightingRenderModule::~cLightingRenderModule()
{
    cViewportQuadGeometry::Destroy(ppLogicalDevice);
}

void cLightingRenderModule::CreateUniformHandler()
{
    // Create the uniform handler. This is responsible for the uniform variables
    ppUniformHandler = new cLightingUniformHandler(ppLogicalDevice, ppSwapChain);
}

void cLightingRenderModule::CreateRenderPass()
{
    // Create the render pass. This holds information about the frames we want to render.
    // will probably be moved somewhere else later
    ppRenderPass = new cLightingRenderPass(ppLogicalDevice, ppSwapChain);
}

void cLightingRenderModule::CreatePipeline(std::vector<string>& aShaders)
{
    // Create the graphics pipeline. Handles the shaders and fixed-function operations for the graphics pipeline.
    ppRenderPipeline = new cLightingPipeline(ppSwapChain, ppLogicalDevice, ppRenderPass, ppUniformHandler, aShaders);
}
