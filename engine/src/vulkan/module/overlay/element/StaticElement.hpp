#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/text/Vertex2D.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/geometry/BufferHelper.hpp>
#include <vulkan/texture/Texture.hpp>

struct tElementInfo
{
    uint uiWidth;
    uint uiHeight;
};

class cStaticElement
{
protected:
    cLogicalDevice* ppLogicalDevice;
    VkBuffer poVertexBuffer;
    VkDeviceMemory poVertexBufferMemory;

    // Elemental
    std::vector<tVertex2D> patVertices;
    cTexture* ppTexture;
    tElementInfo ptInfo;
    glm::vec2 ptRotation = glm::vec2(0.0f, 0.0f);
    glm::vec2 ptPosition = glm::vec2(0.0f, 0.0f);
    glm::vec2 ptScale = glm::vec2(1.0f, 1.0f);

public:
    cStaticElement(tElementInfo tInfo, cTexture* pTexture, cLogicalDevice* pLogicalDevice);
    ~cStaticElement();

    virtual void OnLoadVertices();

    virtual VkImageView& GetImageView();
    virtual VkSampler& GetImageSampler();

    void CmdBindVertexBuffer(VkCommandBuffer& oCommandBuffer);
    virtual uint GetVertexCount();

    void SetRotation(const glm::vec2 poRotation);
    void SetPosition(const glm::vec2 poPosition);
    void SetScale(const glm::vec2 poScale);
    void RotateLeft(float fAngleDiff);
    void RotateRight(float fAngleDiff);

    glm::mat4 GetMatrix(cWindow* pWindow);

    virtual void CopyToDevice();
};

cStaticElement::cStaticElement(tElementInfo tInfo, cTexture* pTexture, cLogicalDevice* pLogicalDevice) //-V730
{
    assert(pLogicalDevice != nullptr);

    ptInfo = tInfo;
    ppTexture = pTexture;
    ppLogicalDevice = pLogicalDevice;
}

cStaticElement::~cStaticElement()
{
    ppLogicalDevice->DestroyBuffer(poVertexBuffer, nullptr);
    ppLogicalDevice->FreeMemory(poVertexBufferMemory, nullptr);
}

void cStaticElement::OnLoadVertices()
{
    patVertices.push_back({{0, 0}, {0, 0}});
    patVertices.push_back({{ptInfo.uiWidth, ptInfo.uiHeight}, {1, 1}});
    patVertices.push_back({{ptInfo.uiWidth, 0}, {1, 0}});
    patVertices.push_back({{0, 0}, {0, 0}});
    patVertices.push_back({{0, ptInfo.uiHeight}, {0, 1}});
    patVertices.push_back({{ptInfo.uiWidth, ptInfo.uiHeight}, {1, 1}});
}

void cStaticElement::CopyToDevice()
{
    VkDeviceSize ulVertexBufferSize = sizeof(tVertex2D) * patVertices.size();
    cBufferHelper::CopyToNewBuffer(ppLogicalDevice,
                                   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                   patVertices.data(), ulVertexBufferSize,
                                   poVertexBuffer, poVertexBufferMemory);
}

void cStaticElement::CmdBindVertexBuffer(VkCommandBuffer& oCommandBuffer)
{
    VkDeviceSize ulOffset = 0;
    vkCmdBindVertexBuffers(oCommandBuffer, 0, 1, &poVertexBuffer, &ulOffset);
}

void cStaticElement::SetRotation(glm::vec2 oRotation)
{
    ptRotation.x = oRotation.x;
    ptRotation.y = oRotation.y;
}

void cStaticElement::SetPosition(glm::vec2 oPosition)
{
    ptPosition.x = oPosition.x;
    ptPosition.y = oPosition.y;
}

void cStaticElement::SetScale(glm::vec2 oScale)
{
    ptScale.x = oScale.x;
    ptScale.y = oScale.y;
}

void cStaticElement::RotateLeft(float fAngleDiff)
{
    if (ptRotation.y >= 360.0f)
        ptRotation.y = 0;
    ptRotation.y += fAngleDiff;
}

void cStaticElement::RotateRight(float fAngleDiff)
{
    if (ptRotation.y < 0.0f)
        ptRotation.y = 360.0f;
    ptRotation.y -= fAngleDiff;
}

glm::mat4 cStaticElement::GetMatrix(cWindow* pWindow)
{
    glm::mat4 oModel(1.0f);

    // Translation
    oModel = glm::translate(oModel, glm::vec3(ptPosition, 0));

    // Scaling
    oModel = glm::scale(oModel, glm::vec3(ptScale, 1));

    // Rotation around X axis
    if (ptRotation.x > 0.0)
    {
        const glm::vec3 oRot_x = glm::vec3(1.0f, 0.0f, 0.0f);
        oModel = glm::rotate(oModel, glm::radians(ptRotation.x), oRot_x);
    }

    // Rotation around Y axis
    if (ptRotation.y > 0.0)
    {
        const glm::vec3 oRot_y = glm::vec3(0.0f, 0.0f, 1.0f);
        oModel = glm::rotate(oModel, glm::radians(ptRotation.y), oRot_y);
    }

    glm::mat4 proj = glm::ortho(0.0f, (float) WIDTH, 0.0f, (float) HEIGHT);

    return proj * oModel;
}

VkImageView& cStaticElement::GetImageView()
{
    return ppTexture->GetView();
}

VkSampler& cStaticElement::GetImageSampler()
{
    return ppTexture->GetSampler();
}

uint cStaticElement::GetVertexCount()
{
    return (uint) patVertices.size();
}
