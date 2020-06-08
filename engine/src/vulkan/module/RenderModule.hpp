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

    std::vector<string>& paShaders;

public:
    cRenderModule(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain, std::vector<string>& aShaders);
    virtual ~cRenderModule();

    iUniformHandler* GetUniformHandler();
    cRenderPass* GetRenderPass();
    cRenderPipeline* GetRenderPipeline();

    void RebuildRenderPass();

protected:
    void Init();

    virtual void CreateUniformHandler() = 0;
    virtual void CreateRenderPass() = 0;
    virtual void CreatePipeline(std::vector<string>& aShaders) = 0;
};

cRenderModule::cRenderModule(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain, std::vector<string>& aShaders)
        : paShaders(aShaders)
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

    CreatePipeline(paShaders);
    assert(ppRenderPipeline != nullptr); // pipeline should be created

    ENGINE_LOG("Render module '" << CURRENT_CLASS_NAME << "' initialized");
}

cRenderModule::~cRenderModule()
{
    if (ppRenderPipeline != nullptr) //-V809
    {
        delete ppRenderPipeline;
    }
    if (ppRenderPass != nullptr) //-V809
    {
        delete ppRenderPass;
    }
    if (ppUniformHandler != nullptr) //-V809
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

void cRenderModule::RebuildRenderPass()
{
    delete ppRenderPass;
    CreateRenderPass();
    assert(ppRenderPass != nullptr);
    assert(ppRenderPass->GetRenderPass() != VK_NULL_HANDLE);
}
