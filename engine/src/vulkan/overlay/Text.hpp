#pragma once

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

    int puiNumLetters = 0;

public:
    cText(cLogicalDevice* pLogicalDevice);
    ~cText();

    void UpdateText(string sText, float fFontSize, stb_fontchar* stbFontData);
    void BindVertexBuffer(VkCommandBuffer& oCommandBuffer);
    uint GetNumLetters();
};

cText::cText(cLogicalDevice* pLogicalDevice)
{
    ppLogicalDevice = pLogicalDevice;

    VkDeviceSize uiSize = 2048 * sizeof(tVertex2D);
    cBufferHelper::CreateBuffer(pLogicalDevice, uiSize,
                                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                poBuffer, poBufferMemory);
}

cText::~cText()
{
    ppLogicalDevice->DestroyBuffer(poBuffer, nullptr);
    ppLogicalDevice->FreeMemory(poBufferMemory, nullptr);
}

void cText::UpdateText(string sText, float fFontSize, stb_fontchar* stbFontData)
{
    tVertex2D* mapped;
    ppLogicalDevice->MapMemory(poBufferMemory, 0, VK_WHOLE_SIZE, 0, (void**) &mapped);

    float x = 10;
    float y = 10;

    const uint32_t firstChar = STB_FONT_consolas_24_latin1_FIRST_CHAR;

    assert(mapped != nullptr);

    const float charW = fFontSize / 2500;
    const float charH = fFontSize / 1300;

    float fbW = (float) 2500;
    float fbH = (float) 1300;
    x = (x / fbW * 2.0f) - 1.0f;
    y = (y / fbH * 2.0f) - 1.0f;

    // Calculate text width
    float textWidth = 0;
    for (auto letter : sText)
    {
        stb_fontchar* charData = &stbFontData[(uint32_t) letter - firstChar];
        textWidth += charData->advance * charW;
    }

    puiNumLetters = 0;

    // Generate a uv mapped quad per char in the new text
    for (auto letter : sText)
    {
        stb_fontchar* charData = &stbFontData[(uint32_t) letter - firstChar];

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
