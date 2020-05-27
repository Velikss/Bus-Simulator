#pragma once

#define MAX_CHARACTER_COUNT 512

#include <pch.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/geometry/BufferHelper.hpp>
#include <vendor/stb_font_consolas_24_latin1.inl>
#include "Vertex2D.hpp"

class cText
{
private:
    VkBuffer poBuffer;
    VkDeviceMemory poBufferMemory;

    cLogicalDevice* ppLogicalDevice;
    cWindow* ppWindow;

    int puiNumLetters = 0;

    VkDeviceSize pulBufferSize;

public:
    cText(cLogicalDevice* pLogicalDevice, cWindow* pWindow);
    ~cText();

    void UpdateText(const string& sText, float fFontSize, stb_fontchar* stbFontData, uint iPosX, uint iPosY);
    void BindVertexBuffer(VkCommandBuffer& oCommandBuffer);
    uint GetNumLetters();
};

cText::cText(cLogicalDevice* pLogicalDevice, cWindow* pWindow)
{
    assert(pLogicalDevice != nullptr);
    assert(pWindow != nullptr);

    ppLogicalDevice = pLogicalDevice;
    ppWindow = pWindow;

    // Create a buffer for the text data
    pulBufferSize = MAX_CHARACTER_COUNT * 4 * sizeof(tVertex2D);
    assert(pulBufferSize > 0); // must be enough room for at least one character
    cBufferHelper::CreateBuffer(pLogicalDevice, pulBufferSize,
                                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                poBuffer, poBufferMemory);
}

cText::~cText()
{
    ppLogicalDevice->DestroyBuffer(poBuffer, nullptr);
    ppLogicalDevice->FreeMemory(poBufferMemory, nullptr);
}

void cText::UpdateText(const string& sText, float fFontSize, stb_fontchar* stbFontData, uint iPosX, uint iPosY) //-V813
{
    const uint uiScreenWidth = WIDTH;
    const uint uiScreenHeight = HEIGHT;

    assert(sText.size() <= MAX_CHARACTER_COUNT);
    assert(fFontSize > 0);          // font size can't be 0
    assert(stbFontData != nullptr); // font data must exist
    assert(iPosX < uiScreenWidth);  // x must be less than the screen width
    assert(iPosY < uiScreenHeight); // y must be less than the screen height

    float x = (float) iPosX;
    float y = (float) iPosY;

    const uint32_t firstChar = STB_FONT_arial_50_usascii_FIRST_CHAR;

    // Calculate text size
    const float charW = fFontSize / uiScreenWidth;
    const float charH = fFontSize / uiScreenHeight;

    float fbW = (float) uiScreenWidth;
    float fbH = (float) uiScreenHeight;
    x = (x / fbW * 2.0f) - 1.0f;
    y = (y / fbH * 2.0f) - 1.0f;

    // Calculate text width
    float textWidth = 0;
    for (auto letter : sText)
    {
        stb_fontchar* charData = &stbFontData[(uint32_t) letter - firstChar]; //-V108
        textWidth += charData->advance * charW;
    }

    puiNumLetters = 0;

    // Map the memory for the text buffer to a pointer
    tVertex2D* mapped;
    ppLogicalDevice->MapMemory(poBufferMemory, 0, pulBufferSize, 0, (void**) &mapped);

    assert(mapped != nullptr);

    // Generate a uv mapped quad per char in the new text
    for (auto letter : sText)
    {
        stb_fontchar* charData = &stbFontData[(uint32_t) letter - firstChar]; //-V108

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
    ppLogicalDevice->UnmapMemory(poBufferMemory);
}

void cText::BindVertexBuffer(VkCommandBuffer& oCommandBuffer)
{
    // We don't want to use any special offset for the buffer
    VkDeviceSize ulOffset = 0;

    vkCmdBindVertexBuffers(oCommandBuffer, 0, 1, &poBuffer, &ulOffset);
    vkCmdBindVertexBuffers(oCommandBuffer, 1, 1, &poBuffer, &ulOffset);
}

uint cText::GetNumLetters()
{
    return puiNumLetters;
}
