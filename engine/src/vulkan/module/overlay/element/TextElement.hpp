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
public:
    void OnLoadVertices() override;
    VkDeviceSize GetMemorySize() override;
    void FillMemory(void* pMemory) override;
    uint GetVertexCount() override;

    VkImageView& GetImageView() override;
    VkSampler& GetImageSampler() override;

    void SetFont(float fFontSize, cFont* pFont, glm::vec3 tColor);
    void UpdateText(const string& sText);
    glm::vec3 GetColor();
};

void cTextElement::OnLoadVertices()
{
}

VkDeviceSize cTextElement::GetMemorySize()
{
    return MAX_CHARACTER_COUNT * 4 * sizeof(tVertex2D);
}

void cTextElement::FillMemory(void* pMemory)
{
    const uint firstChar = STB_FONT_arial_50_usascii_FIRST_CHAR;

    // Calculate text size
    const float charW = pfFontSize / 10;
    const float charH = pfFontSize / 10;

    // Calculate text width
    float textWidth = 0;
    for (auto letter : psText)
    {
        stb_fontchar* charData = &ppFont->ppFontData[(uint) letter - firstChar]; //-V108
        textWidth += charData->advance * charW;
    }

    // Map the memory for the text buffer to a pointer
    tVertex2D* pMapped = reinterpret_cast<tVertex2D*>(pMemory);

    assert(pMapped != nullptr);

    float x = 0;

    // Generate a uv mapped quad per char in the new text
    for (auto letter : psText)
    {
        stb_fontchar* charData = &ppFont->ppFontData[(uint) letter - firstChar]; //-V108

        pMapped->pos.x = (x + (float) charData->x0 * charW);
        pMapped->pos.y = (0 + (float) charData->y0 * charH);
        pMapped->texCoord.x = charData->s0;
        pMapped->texCoord.y = charData->t0;
        pMapped++;

        pMapped->pos.x = (x + (float) charData->x1 * charW);
        pMapped->pos.y = (0 + (float) charData->y0 * charH);
        pMapped->texCoord.x = charData->s1;
        pMapped->texCoord.y = charData->t0;
        pMapped++;

        pMapped->pos.x = (x + (float) charData->x0 * charW);
        pMapped->pos.y = (0 + (float) charData->y1 * charH);
        pMapped->texCoord.x = charData->s0;
        pMapped->texCoord.y = charData->t1;
        pMapped++;

        pMapped->pos.x = (x + (float) charData->x1 * charW);
        pMapped->pos.y = (0 + (float) charData->y1 * charH);
        pMapped->texCoord.x = charData->s1;
        pMapped->texCoord.y = charData->t1;
        pMapped++;

        x += charData->advance * charW;
    }

    puiNumLetters = psText.length();
}

uint cTextElement::GetVertexCount()
{
    return puiNumLetters * 4;
}

VkImageView& cTextElement::GetImageView()
{
    return ppFont->poFontImageView;
}

VkSampler& cTextElement::GetImageSampler()
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
}

void cTextElement::UpdateText(const string& sText)
{
    assert(ppFont != nullptr);
    psText = sText;
    Invalidate();
}

glm::vec3 cTextElement::GetColor()
{
    return ptColor;
}
