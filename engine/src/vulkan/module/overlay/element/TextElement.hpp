#pragma once

#define MAX_CHARACTER_COUNT 256

#include <pch.hpp>
#include <vulkan/module/overlay/element/StaticElement.hpp>
#include <vulkan/module/overlay/text/Font.hpp>

class cTextElement : public cUIElement
{
private:
    string psText = "";
    uint puiNumLetters = 0;

    float pfFontSize = 3.0f;
    cFont* ppFont = nullptr;
    glm::vec3 ptColor;
    uint puiWidth;

public:
    void OnLoadVertices() override;
    VkDeviceSize GetMemorySize(uint uiIndex) override;
    void InitializeMemory(void* pMemory, uint uiIndex) override;
    uint GetVertexCount(uint uiIndex) override;

    VkImageView& GetImageView(uint uiIndex) override;
    VkSampler& GetImageSampler(uint uiIndex) override;

    void SetFont(float fFontSize, cFont* pFont, glm::vec3 tColor);
    void SetFont(const tFontInfo& tFont);
    void UpdateText(const string& sText);

    uint GetChildCount() override;
    bool IsTextElement(uint uiIndex) override;
    glm::vec3 GetColor(uint uiIndex) override;

    static uint GetTextWidth(string sString, cFont* pFont, float fFontSize);
};

void cTextElement::OnLoadVertices()
{
    ptInfo.uiWidth = GetTextWidth(psText, ppFont, pfFontSize);
    uint uiLineCount = 1;
    for (char c : psText) if (c == '\n') uiLineCount++;
    ptInfo.uiHeight = ppFont->GetFontHeight(pfFontSize) * uiLineCount;
}

VkDeviceSize cTextElement::GetMemorySize(uint uiIndex)
{
    return MAX_CHARACTER_COUNT * 4 * sizeof(tVertex2D);
}

void cTextElement::InitializeMemory(void* pMemory, uint uiIndex)
{
    const uint firstChar = STB_FONT_arial_50_usascii_FIRST_CHAR;

    // Calculate text size
    const float charW = pfFontSize / 10;
    const float charH = pfFontSize / 10;

    // Map the memory for the text buffer to a pointer
    tVertex2D* pMapped = reinterpret_cast<tVertex2D*>(pMemory);

    assert(pMapped != nullptr);

    float x = 0;
    float y = 0;

    puiNumLetters = 0;

    // Generate a uv mapped quad per char in the new text
    for (auto letter : psText)
    {
        if (letter == '\n')
        {
            y += charH * 50;
            x = 0;
            continue;
        }

        stb_fontchar* charData = &ppFont->ppFontData[(uint) letter - firstChar]; //-V108

        pMapped->pos.x = (x + (float) charData->x0 * charW);
        pMapped->pos.y = (y + (float) charData->y0 * charH);
        pMapped->texCoord.x = charData->s0;
        pMapped->texCoord.y = charData->t0;
        pMapped++;

        pMapped->pos.x = (x + (float) charData->x0 * charW);
        pMapped->pos.y = (y + (float) charData->y1 * charH);
        pMapped->texCoord.x = charData->s0;
        pMapped->texCoord.y = charData->t1;
        pMapped++;

        pMapped->pos.x = (x + (float) charData->x1 * charW);
        pMapped->pos.y = (y + (float) charData->y1 * charH);
        pMapped->texCoord.x = charData->s1;
        pMapped->texCoord.y = charData->t1;
        pMapped++;

        pMapped->pos.x = (x + (float) charData->x0 * charW);
        pMapped->pos.y = (y + (float) charData->y0 * charH);
        pMapped->texCoord.x = charData->s0;
        pMapped->texCoord.y = charData->t0;
        pMapped++;

        pMapped->pos.x = (x + (float) charData->x1 * charW);
        pMapped->pos.y = (y + (float) charData->y1 * charH);
        pMapped->texCoord.x = charData->s1;
        pMapped->texCoord.y = charData->t1;
        pMapped++;

        pMapped->pos.x = (x + (float) charData->x1 * charW);
        pMapped->pos.y = (y + (float) charData->y0 * charH);
        pMapped->texCoord.x = charData->s1;
        pMapped->texCoord.y = charData->t0;
        pMapped++;

        x += charData->advance * charW;
        puiNumLetters++;
    }

    puiWidth = ((uint) x) + 1;
}

uint cTextElement::GetVertexCount(uint uiIndex)
{
    return puiNumLetters * 6;
}

VkImageView& cTextElement::GetImageView(uint uiIndex)
{
    return ppFont->poFontImageView;
}

VkSampler& cTextElement::GetImageSampler(uint uiIndex)
{
    return ppFont->poFontImageSampler;
}

void cTextElement::SetFont(float fFontSize, cFont* pFont, glm::vec3 tColor)
{
    assert(fFontSize > 0);
    assert(pFont != nullptr);

    pfFontSize = fFontSize;
    ppFont = pFont;
    ptColor = tColor;

    ptInfo.uiWidth = GetTextWidth(psText, ppFont, pfFontSize);
    ptInfo.uiHeight = ppFont->GetFontHeight(pfFontSize);
}

void cTextElement::SetFont(const tFontInfo& tFont)
{
    SetFont(tFont.pfFontSize, tFont.ppFont, tFont.ptFontColor);
}

void cTextElement::UpdateText(const string& sText)
{
    assert(ppFont != nullptr);
    psText = sText;
    ptInfo.uiWidth = GetTextWidth(psText, ppFont, pfFontSize);
    Invalidate();
}

uint cTextElement::GetChildCount()
{
    return 1;
}

bool cTextElement::IsTextElement(uint uiIndex)
{
    return true;
}

glm::vec3 cTextElement::GetColor(uint uiIndex)
{
    return ptColor;
}

uint cTextElement::GetTextWidth(string sString, cFont* pFont, float fFontSize)
{
    const uint firstChar = STB_FONT_arial_50_usascii_FIRST_CHAR;

    // Calculate character size
    const float charW = fFontSize / 10;

    // Calculate text width
    float fTextWidth = 0;
    for (auto letter : sString)
    {
        if (letter == '\n') break;
        stb_fontchar* charData = &pFont->ppFontData[(uint) letter - firstChar]; //-V108
        fTextWidth += charData->advance * charW;
    }

    return ((uint) fTextWidth) + 1;
}
