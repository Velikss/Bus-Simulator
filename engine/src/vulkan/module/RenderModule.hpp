#pragma once

#include <pch.hpp>
#include <vulkan/renderpass/RenderPass.hpp>
#include <vulkan/pipeline/RenderPipeline.hpp>
#include <vulkan/uniform/UniformHandler.hpp>
#include <vulkan/command/CommandBufferRecorder.hpp>

class cRenderModule
{
protected:
    iUniformHandler* ppUniformHandler = nullptr;
    cRenderPass* ppRenderPass = nullptr;
    cRenderPipeline* ppRenderPipeline = nullptr;

    cLogicalDevice* ppLogicalDevice = nullptr;
    cSwapChain* ppSwapChain = nullptr;

public:
    cRenderModule(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain);
    virtual ~cRenderModule();

    iUniformHandler* GetUniformHandler();
    cRenderPass* GetRenderPass();
    cRenderPipeline* GetRenderPipeline();

protected:
    void Init();

    virtual void CreateUniformHandler() = 0;
    virtual void CreateRenderPass() = 0;
    virtual void CreatePipeline() = 0;
};

cRenderModule::cRenderModule(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain)
{
    assert(pLogicalDevice != nullptr);
    assert(pSwapChain != nullptr);

    ppLogicalDevice = pLogicalDevice;
    ppSwapChain = pSwapChain;
}

void cRenderModule::Init()
{
    CreateUniformHandler();
    assert(ppUniformHandler != nullptr); // uniform handler should be created

    CreateRenderPass();
    assert(ppRenderPass != nullptr); // render pass should be created

    CreatePipeline();
    assert(ppRenderPipeline != nullptr); // pipeline should be created
}

cRenderModule::~cRenderModule()
{
    if (ppRenderPipeline != nullptr)
    {
        delete ppRenderPipeline;
    }
    if (ppRenderPass != nullptr)
    {
        delete ppRenderPass;
    }
    if (ppUniformHandler != nullptr)
    {
        delete ppUniformHandler;
    }
}

iUniformHandler* cRenderModule::GetUniformHandler()
{
    assert(ppUniformHandler != nullptr);

    return ppUniformHandler;
}

cRenderPass* cRenderModule::GetRenderPass()
{
    assert(ppRenderPass != nullptr);

    return ppRenderPass;
}

cRenderPipeline* cRenderModule::GetRenderPipeline()
{
    assert(ppRenderPipeline != nullptr);

    return ppRenderPipeline;
}
