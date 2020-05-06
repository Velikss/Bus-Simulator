#pragma once

#include <pch.hpp>
#include "OverlayPipeline.hpp"
#include "OverlayRenderPass.hpp"
#include "OverlayUniformHandler.hpp"
#include "OverlayCommandBufferRecorder.hpp"

class TextTest
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
    TextTest(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain);
    ~TextTest();

    void UpdateText(string sText);

    iCommandBufferRecorder* GetCommandRecorder();
    iUniformHandler* GetUniformHandler();
};

TextTest::TextTest(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain)
{
    stb_font_arial_50_usascii(stbFontData, font24pixels, fontHeight);
    ppFont = new cFont(pLogicalDevice, fontWidth, fontHeight, &font24pixels[0][0]);

    ppUniformHandler = new cOverlayUniformHandler(pLogicalDevice, ppFont);
    ppUniformHandler->SetupUniformBuffers(nullptr, nullptr);
    ppRenderPass = new cOverlayRenderPass(pLogicalDevice, pSwapChain);
    ppPipeline = new cOverlayPipeline(pSwapChain, pLogicalDevice, ppRenderPass, ppUniformHandler);
    ppText = new cText(pLogicalDevice);

    ppCommandRecorder = new cOverlayCommandBufferRecorder(ppRenderPass, pSwapChain, ppPipeline,
                                                          ppUniformHandler, ppText);
}

TextTest::~TextTest()
{
    delete ppFont;
    delete ppUniformHandler;
    delete ppRenderPass;
    delete ppPipeline;
    delete ppText;
}

void TextTest::UpdateText(string sText)
{
    ppText->UpdateText(sText, 1.5f, stbFontData);
}

iCommandBufferRecorder* TextTest::GetCommandRecorder()
{
    return ppCommandRecorder;
}

iUniformHandler* TextTest::GetUniformHandler()
{
    return ppUniformHandler;
}
