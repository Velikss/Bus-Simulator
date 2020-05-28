#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/text/Vertex2D.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/geometry/BufferHelper.hpp>
#include <vulkan/texture/Texture.hpp>
#include <vulkan/module/overlay/element/UIElement.hpp>

struct tElementInfo
{
    uint uiWidth;
    uint uiHeight;
};

class cStaticElement : public cUIElement
{
protected:
    std::vector<tVertex2D> patVertices;

    cTexture* ppTexture;
    tElementInfo ptInfo;

public:
    cStaticElement(tElementInfo tInfo, cTexture* pTexture);

    void OnLoadVertices() override;
    VkDeviceSize GetMemorySize() override;
    void FillMemory(void* pMemory) override;
    uint GetVertexCount() override;
    VkImageView& GetImageView() override;
    VkSampler& GetImageSampler() override;
};

cStaticElement::cStaticElement(tElementInfo tInfo, cTexture* pTexture) //-V730
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

VkDeviceSize cStaticElement::GetMemorySize()
{
    return patVertices.size() * sizeof(tVertex2D);
}

void cStaticElement::FillMemory(void* pMemory)
{
    memcpy(pMemory, patVertices.data(), GetMemorySize());
}

uint cStaticElement::GetVertexCount()
{
    return patVertices.size();
}

VkImageView& cStaticElement::GetImageView()
{
    return ppTexture->GetView();
}

VkSampler& cStaticElement::GetImageSampler()
{
    return ppTexture->GetSampler();
}
