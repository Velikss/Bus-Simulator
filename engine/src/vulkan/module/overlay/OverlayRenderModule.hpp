#pragma once

#include <pch.hpp>
#include <vulkan/module/RenderModule.hpp>
#include <vulkan/module/overlay/OverlayUniformHandler.hpp>
#include <vulkan/module/overlay/OverlayRenderPass.hpp>
#include <vulkan/module/overlay/OverlayPipeline.hpp>
#include <vulkan/module/overlay/OverlayCommandBufferRecorder.hpp>

class cOverlayRenderModule : public cRenderModule, public iCommandRecorderProvider
{
public:
    static cFont* FONT;

private:
    static const uint fontWidth = STB_FONT_arial_50_usascii_BITMAP_WIDTH;
    static const uint fontHeight = STB_FONT_arial_50_usascii_BITMAP_HEIGHT;

    stb_fontchar stbFontData[STB_FONT_arial_50_usascii_NUM_CHARS];
    byte font24pixels[fontHeight][fontWidth];

    cWindow* ppWindow;
    iGameManager* ppOverlayProvider;

    iCommandBufferRecorder* ppCommandRecorder;

public:
    cOverlayRenderModule(cLogicalDevice* pLogicalDevice,
                         cSwapChain* pSwapChain, cWindow* pWindow,
                         std::vector<string>& aShaders,
                         iGameManager* pOverlayProvider);
    virtual ~cOverlayRenderModule();

    void CreateCommandRecorder();
    iCommandBufferRecorder* GetCommandRecorder() override;

protected:
    void CreateUniformHandler() override;
    void CreateRenderPass() override;
    void CreatePipeline(std::vector<string>& aShaders) override;

private:
    void LoadFont();
};

cFont* cOverlayRenderModule::FONT = nullptr;

cOverlayRenderModule::cOverlayRenderModule(cLogicalDevice* pLogicalDevice,
                                           cSwapChain* pSwapChain, cWindow* pWindow,
                                           std::vector<string>& aShaders,
                                           iGameManager* pOverlayProvider)
        : cRenderModule(pLogicalDevice, pSwapChain, aShaders)
{
    assert(pWindow != nullptr);
    assert(pOverlayProvider != nullptr);

    ppWindow = pWindow;
    ppOverlayProvider = pOverlayProvider;

    LoadFont();
    Init();
}

void cOverlayRenderModule::LoadFont()
{
    stb_font_arial_50_usascii(stbFontData, font24pixels, fontHeight);
    FONT = new cFont(ppLogicalDevice, fontWidth, fontHeight, &font24pixels[0][0], stbFontData);
}

void cOverlayRenderModule::CreateUniformHandler()
{
    ppUniformHandler = new cOverlayUniformHandler(ppLogicalDevice, FONT, ppWindow, ppOverlayProvider);
}

void cOverlayRenderModule::CreateRenderPass()
{
    ppRenderPass = new cOverlayRenderPass(ppLogicalDevice, ppSwapChain);
}

void cOverlayRenderModule::CreatePipeline(std::vector<string>& aShaders)
{
    ppRenderPipeline = new cOverlayPipeline(ppSwapChain, ppLogicalDevice, ppRenderPass, ppUniformHandler, aShaders);
}

void cOverlayRenderModule::CreateCommandRecorder()
{
    ppCommandRecorder = new cOverlayCommandBufferRecorder(ppRenderPass, ppSwapChain, ppRenderPipeline,
                                                          ppUniformHandler, ppOverlayProvider);
}

iCommandBufferRecorder* cOverlayRenderModule::GetCommandRecorder()
{
    return ppCommandRecorder;
}

cOverlayRenderModule::~cOverlayRenderModule()
{
    delete FONT;
}
