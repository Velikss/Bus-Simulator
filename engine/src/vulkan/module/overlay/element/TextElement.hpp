#pragma once

#define MAX_CHARACTER_COUNT 512

#include <pch.hpp>
#include <vulkan/module/overlay/element/StaticElement.hpp>
#include <vulkan/module/overlay/text/Font.hpp>

class cTextElement : public cStaticElement
{
private:
    int puiNumLetters = 0;

    VkDeviceSize pulBufferSize;

    string psText = "";

    float pfFontSize = 3.0f;
    cFont* ppFont = nullptr;
    glm::vec3 ptColor;
    static bool pbInValidated;
public:
    cTextElement(cLogicalDevice* pLogicalDevice);
    void LoadVertices() override;

    void SetFont(float fFontSize, cFont* pFont, glm::vec3 tColor);
    void UpdateText(const string& sText);
    VkImageView& GetImageView() override;
    VkSampler& GetImageSampler() override;
    uint GetVertexCount() override;
    glm::vec3 GetColor();
    static bool Invalidated();
    static void Validate();
    static void Invalidate();
private:
    void CopyToDevice() override;

};

bool cTextElement::pbInValidated = true;

cTextElement::cTextElement(cLogicalDevice* pLogicalDevice)
        : cStaticElement({0, 0}, nullptr, pLogicalDevice)
{
    // Create a buffer for the text data
    pulBufferSize = MAX_CHARACTER_COUNT * 4 * sizeof(tVertex2D);
    assert(pulBufferSize > 0); // must be enough room for at least one character
    cBufferHelper::CreateBuffer(pLogicalDevice, pulBufferSize,
                                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                poVertexBuffer, poVertexBufferMemory);
}

void cTextElement::LoadVertices()
{
    UpdateText(psText);
}

void cTextElement::CopyToDevice()
{
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

    assert(sText.size() <= MAX_CHARACTER_COUNT);

    float x = 0;
    float y = 0;

    const uint firstChar = STB_FONT_arial_50_usascii_FIRST_CHAR;

    // Calculate text size
    const float charW = pfFontSize / 10;
    const float charH = pfFontSize / 10;

    // Calculate text width
    float textWidth = 0;
    for (auto letter : sText)
    {
        stb_fontchar* charData = &ppFont->ppFontData[(uint) letter - firstChar]; //-V108
        textWidth += charData->advance * charW;
    }

    puiNumLetters = 0;

    // Map the memory for the text buffer to a pointer
    tVertex2D* mapped;
    ppLogicalDevice->MapMemory(poVertexBufferMemory, 0, pulBufferSize, 0, (void**) &mapped);

    assert(mapped != nullptr);

    // Generate a uv mapped quad per char in the new text
    for (auto letter : sText)
    {
        stb_fontchar* charData = &ppFont->ppFontData[(uint) letter - firstChar]; //-V108

        mapped->pos.x = (x + (float) charData->x0 * charW);
        mapped->pos.y = (y + (float) charData->y0 * charH);
        mapped->texCoord.x = charData->s0;
        mapped->texCoord.y = charData->t0;
        mapped++;

        mapped->pos.x = (x + (float) charData->x1 * charW);
        mapped->pos.y = (y + (float) charData->y0 * charH);
        mapped->texCoord.x = charData->s1;
        mapped->texCoord.y = charData->t0;
        mapped++;

        mapped->pos.x = (x + (float) charData->x0 * charW);
        mapped->pos.y = (y + (float) charData->y1 * charH);
        mapped->texCoord.x = charData->s0;
        mapped->texCoord.y = charData->t1;
        mapped++;

        mapped->pos.x = (x + (float) charData->x1 * charW);
        mapped->pos.y = (y + (float) charData->y1 * charH);
        mapped->texCoord.x = charData->s1;
        mapped->texCoord.y = charData->t1;
        mapped++;

        x += charData->advance * charW;

        puiNumLetters++;
    }

    // Unmap the memory again
    ppLogicalDevice->UnmapMemory(poVertexBufferMemory);

    Invalidate();
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

glm::vec3 cTextElement::GetColor()
{
    return ptColor;
}

bool cTextElement::Invalidated()
{
    return pbInValidated;
}

void cTextElement::Invalidate()
{
    pbInValidated = true;
}

void cTextElement::Validate()
{
    pbInValidated = false;
}
