#pragma once

#include <pch.hpp>
#include "OverlayPipeline.hpp"
#include "OverlayRenderPass.hpp"
#include "OverlayUniformHandler.hpp"
#include "OverlayCommandBufferRecorder.hpp"

class cOverlayHandler
{
private:
    static const uint fontWidth = STB_FONT_arial_50_usascii_BITMAP_WIDTH;
    static const uint fontHeight = STB_FONT_arial_50_usascii_BITMAP_HEIGHT;

    stb_fontchar stbFontData[STB_FONT_arial_50_usascii_NUM_CHARS];
    byte font24pixels[fontHeight][fontWidth];

    cFont* ppFont;
    iUniformHandler* ppUniformHandler;
    cRenderPass* ppRenderPass;
    cOverlayPipeline* ppPipeline;
    cText* ppText;

    cOverlayCommandBufferRecorder* ppCommandRecorder;

public:
    cOverlayHandler(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain, cWindow* pWindow);
    ~cOverlayHandler();

    void UpdateText(string sText);

    iCommandBufferRecorder* GetCommandRecorder();
    iUniformHandler* GetUniformHandler();
};

cOverlayHandler::cOverlayHandler(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain, cWindow* pWindow)
{
    assert(pLogicalDevice != nullptr);
    assert(pSwapChain != nullptr);
    assert(pWindow != nullptr);

    stb_font_arial_50_usascii(stbFontData, font24pixels, fontHeight);
    ppFont = new cFont(pLogicalDevice, fontWidth, fontHeight, &font24pixels[0][0]);

    ppUniformHandler = new cOverlayUniformHandler(pLogicalDevice, ppFont);
    ppUniformHandler->SetupUniformBuffers(nullptr, nullptr);
    ppRenderPass = new cOverlayRenderPass(pLogicalDevice, pSwapChain);
    ppPipeline = new cOverlayPipeline(pSwapChain, pLogicalDevice, ppRenderPass, ppUniformHandler);
    ppText = new cText(pLogicalDevice, pWindow);

    ppText->UpdateText("Loading...", 2.5f, stbFontData, (pWindow->WIDTH / 2) - 100, pWindow->HEIGHT / 2);

    ppCommandRecorder = new cOverlayCommandBufferRecorder(ppRenderPass, pSwapChain, ppPipeline,
                                                          ppUniformHandler, ppText);
}

cOverlayHandler::~cOverlayHandler()
{
    delete ppFont;
    delete ppUniformHandler;
    delete ppRenderPass;
    delete ppPipeline;
    delete ppText;
}

void cOverlayHandler::UpdateText(string sText)
{
    ppText->UpdateText(sText, 1.5f, stbFontData, 10, 10);
}

iCommandBufferRecorder* cOverlayHandler::GetCommandRecorder()
{
    return ppCommandRecorder;
}

iUniformHandler* cOverlayHandler::GetUniformHandler()
{
    return ppUniformHandler;
}
