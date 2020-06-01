#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/element/UIElement.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/Window.hpp>
#include <vulkan/module/overlay/element/TextElement.hpp>

class cCompoundElement : public cUIElement
{
protected:
    std::vector<cUIElement*> papChildren;

public:
    uint GetChildCount() override;

    bool IsTextElement(uint uiIndex) override;
    glm::vec3 GetColor(uint uiIndex) override;

    void OnLoadVertices() override;
    VkDeviceSize GetMemorySize(uint uiIndex);
    void FillMemory(void* pMemory, uint uiIndex) override;
    uint GetVertexCount(uint uiIndex) override;

    VkImageView& GetImageView(uint uiIndex);
    VkSampler& GetImageSampler(uint uiIndex);

    glm::mat4 GetMatrix(cWindow* pWindow, uint uiIndex);
    bool Invalidated() override;
};

void cCompoundElement::OnLoadVertices()
{
    for (cUIElement* pElement : papChildren)
    {
        pElement->OnLoadVertices();
    }
}

uint cCompoundElement::GetChildCount()
{
    return papChildren.size();
}

bool cCompoundElement::IsTextElement(uint uiIndex)
{
    return dynamic_cast<cTextElement*>(papChildren[uiIndex]) != nullptr;
}

glm::vec3 cCompoundElement::GetColor(uint uiIndex)
{
    return papChildren[uiIndex]->GetColor(0);
}

VkDeviceSize cCompoundElement::GetMemorySize(uint uiIndex)
{
    return papChildren[uiIndex]->GetMemorySize(0);
}

void cCompoundElement::FillMemory(void* pMemory, uint uiIndex)
{
    papChildren[uiIndex]->FillMemory(pMemory, 0);
}

uint cCompoundElement::GetVertexCount(uint uiIndex)
{
    return papChildren[uiIndex]->GetVertexCount(0);
}

VkImageView& cCompoundElement::GetImageView(uint uiIndex)
{
    return papChildren[uiIndex]->GetImageView(0);
}

VkSampler& cCompoundElement::GetImageSampler(uint uiIndex)
{
    return papChildren[uiIndex]->GetImageSampler(0);
}

glm::mat4 cCompoundElement::GetMatrix(cWindow* pWindow, uint uiIndex)
{
    return cUIElement::GetMatrix(pWindow, 0) * papChildren[uiIndex]->GetRawMatrix();
}

bool cCompoundElement::Invalidated()
{
    for (cUIElement* pChild : papChildren)
    {
        if (pChild->Invalidated())
        {
            return true;
        }
    }
    return cCompoundElement::Invalidated();
}
