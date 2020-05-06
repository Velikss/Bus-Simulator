#pragma once

#include <pch.hpp>
#include "OverlayPipeline.hpp"
#include "OverlayRenderPass.hpp"
#include "OverlayUniformHandler.hpp"
#include "OverlayCommandBufferRecorder.hpp"

class cTextHandler
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
    cTextHandler(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain, cWindow* pWindow);
    ~cTextHandler();

    void UpdateText(string sText);

    iCommandBufferRecorder* GetCommandRecorder();
    iUniformHandler* GetUniformHandler();
};

cTextHandler::cTextHandler(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain, cWindow* pWindow)
{
    stb_font_arial_50_usascii(stbFontData, font24pixels, fontHeight);
    ppFont = new cFont(pLogicalDevice, fontWidth, fontHeight, &font24pixels[0][0]);

    ppUniformHandler = new cOverlayUniformHandler(pLogicalDevice, ppFont);
    ppUniformHandler->SetupUniformBuffers(nullptr, nullptr);
    ppRenderPass = new cOverlayRenderPass(pLogicalDevice, pSwapChain);
    ppPipeline = new cOverlayPipeline(pSwapChain, pLogicalDevice, ppRenderPass, ppUniformHandler);
    ppText = new cText(pLogicalDevice);

    ppText->UpdateText("Loading...", 2.5f, stbFontData, (pWindow->WIDTH / 2) - 100, pWindow->HEIGHT / 2);

    ppCommandRecorder = new cOverlayCommandBufferRecorder(ppRenderPass, pSwapChain, ppPipeline,
                                                          ppUniformHandler, ppText);
}

cTextHandler::~cTextHandler()
{
    delete ppFont;
    delete ppUniformHandler;
    delete ppRenderPass;
    delete ppPipeline;
    delete ppText;
}

void cTextHandler::UpdateText(string sText)
{
    ppText->UpdateText(sText, 1.5f, stbFontData, 10, 10);
}

iCommandBufferRecorder* cTextHandler::GetCommandRecorder()
{
    return ppCommandRecorder;
}

iUniformHandler* cTextHandler::GetUniformHandler()
{
    return ppUniformHandler;
}
