#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/text/Text.hpp>
#include <vulkan/module/RenderModule.hpp>
#include <vulkan/module/overlay/OverlayUniformHandler.hpp>
#include <vulkan/module/overlay/OverlayRenderPass.hpp>
#include <vulkan/module/overlay/OverlayPipeline.hpp>
#include <vulkan/module/overlay/OverlayCommandBufferRecorder.hpp>

class cOverlayRenderModule : public cRenderModule
{
private:
    static const uint fontWidth = STB_FONT_arial_50_usascii_BITMAP_WIDTH;
    static const uint fontHeight = STB_FONT_arial_50_usascii_BITMAP_HEIGHT;

    stb_fontchar stbFontData[STB_FONT_arial_50_usascii_NUM_CHARS];
    byte font24pixels[fontHeight][fontWidth];

    cFont* ppFont;
    cText* ppText;

    iCommandBufferRecorder* ppCommandRecorder;

public:
    cOverlayRenderModule(cLogicalDevice* pLogicalDevice,
                         cSwapChain* pSwapChain,
                         cWindow* pWindow,
                         std::vector<string>& aShaders);
    virtual ~cOverlayRenderModule();

    void UpdateText(string sText);

    iCommandBufferRecorder* GetCommandRecorder();

protected:
    void CreateUniformHandler() override;
    void CreateRenderPass() override;
    void CreatePipeline(std::vector<string>& aShaders) override;

private:
    void LoadFont();
    void LoadText(cWindow* pWindow);
    void CreateCommandRecorder();
};

cOverlayRenderModule::cOverlayRenderModule(cLogicalDevice* pLogicalDevice,
                                           cSwapChain* pSwapChain,
                                           cWindow* pWindow,
                                           std::vector<string>& aShaders)
        : cRenderModule(pLogicalDevice, pSwapChain, aShaders)
{
    assert(pWindow != nullptr);

    LoadFont();
    LoadText(pWindow);
    Init();
    CreateCommandRecorder();
}

void cOverlayRenderModule::LoadFont()
{
    stb_font_arial_50_usascii(stbFontData, font24pixels, fontHeight);
    ppFont = new cFont(ppLogicalDevice, fontWidth, fontHeight, &font24pixels[0][0]);
}

void cOverlayRenderModule::LoadText(cWindow* pWindow)
{
    ppText = new cText(ppLogicalDevice, pWindow);
    ppText->UpdateText("Loading...", 2.5f, stbFontData,
                       (pWindow->WIDTH / 2) - 100, pWindow->HEIGHT / 2);
}

void cOverlayRenderModule::CreateUniformHandler()
{
    ppUniformHandler = new cOverlayUniformHandler(ppLogicalDevice, ppFont);
    ppUniformHandler->SetupUniformBuffers(nullptr, nullptr);
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
                                                          ppUniformHandler, ppText);
}

void cOverlayRenderModule::UpdateText(string sText)
{
    ppText->UpdateText(sText, 1.5f, stbFontData, 10, 10);
}

iCommandBufferRecorder* cOverlayRenderModule::GetCommandRecorder()
{
    return ppCommandRecorder;
}

cOverlayRenderModule::~cOverlayRenderModule()
{
    delete ppFont;
    delete ppText;
}
