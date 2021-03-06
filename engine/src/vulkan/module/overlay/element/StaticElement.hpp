#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/text/Vertex2D.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/geometry/BufferHelper.hpp>
#include <vulkan/texture/Texture.hpp>
#include <vulkan/module/overlay/element/UIElement.hpp>

class cStaticElement : public cUIElement
{
protected:
    std::vector<tVertex2D> patVertices;

    cTexture* ppTexture;
public:
    cStaticElement(tElementInfo tInfo, cTexture* pTexture);

    void OnLoadVertices() override;
    VkDeviceSize GetMemorySize(uint uiIndex) override;
    void InitializeMemory(void* pMemory, uint uiIndex) override;
    uint GetVertexCount(uint uiIndex) override;
    VkImageView& GetImageView(uint uiIndex) override;
    VkSampler& GetImageSampler(uint uiIndex) override;
    uint GetChildCount() override;
    bool IsTextElement(uint uiIndex) override;
    glm::vec3 GetColor(uint uiIndex) override;
    void SetSize(uint uiWidth, uint uiHeight);
};

cStaticElement::cStaticElement(tElementInfo tInfo, cTexture* pTexture)
{
    assert(pTexture != nullptr);

    ptInfo = tInfo;
    ppTexture = pTexture;
}

void cStaticElement::OnLoadVertices()
{
    patVertices.push_back({{0, 0},
                           {0, 0}});
    patVertices.push_back({{ptInfo.uiWidth, ptInfo.uiHeight},
                           {1,              1}});
    patVertices.push_back({{ptInfo.uiWidth, 0},
                           {1,              0}});
    patVertices.push_back({{0, 0},
                           {0, 0}});
    patVertices.push_back({{0, ptInfo.uiHeight},
                           {0, 1}});
    patVertices.push_back({{ptInfo.uiWidth, ptInfo.uiHeight},
                           {1,              1}});
}

VkDeviceSize cStaticElement::GetMemorySize(uint uiIndex)
{
    return patVertices.size() * sizeof(tVertex2D);
}

void cStaticElement::InitializeMemory(void* pMemory, uint uiIndex)
{
    memcpy(pMemory, patVertices.data(), GetMemorySize(uiIndex));
}

uint cStaticElement::GetVertexCount(uint uiIndex)
{
    return (uint)patVertices.size();
}

VkImageView& cStaticElement::GetImageView(uint uiIndex)
{
    return ppTexture->GetView();
}

VkSampler& cStaticElement::GetImageSampler(uint uiIndex)
{
    return ppTexture->GetSampler();
}

uint cStaticElement::GetChildCount()
{
    return 1;
}

bool cStaticElement::IsTextElement(uint uiIndex)
{
    return false;
}

glm::vec3 cStaticElement::GetColor(uint uiIndex)
{
    return glm::vec3();
}

void cStaticElement::SetSize(uint uiWidth, uint uiHeight)
{
    ptInfo.uiWidth = uiWidth;
    ptInfo.uiHeight = uiHeight;
    Invalidate();
}
