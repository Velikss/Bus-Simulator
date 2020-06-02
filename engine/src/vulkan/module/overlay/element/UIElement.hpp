#pragma once

#include <pch.hpp>
#include <vulkan/util/Invalidatable.hpp>
#include <vulkan/module/overlay/FocusHandler.hpp>

class cUIElement : public cInvalidatable, public cFocussable
{
protected:
    iFocusHandler* ppFocusHandler = nullptr;

private:
    cInvalidatable* ppParent = nullptr;

    glm::vec2 ptRotation = glm::vec2(0.0f, 0.0f);
    glm::vec2 ptPosition = glm::vec2(0.0f, 0.0f);
    glm::vec2 ptScale = glm::vec2(1.0f, 1.0f);

public:
    virtual ~cUIElement();
    virtual void SetFocusHandler(iFocusHandler* pFocusHandler);

    virtual uint GetChildCount() = 0;
    virtual bool IsTextElement(uint uiIndex) = 0;
    virtual glm::vec3 GetColor(uint uiIndex) = 0;

    virtual void OnPreLoad();
    virtual void OnLoadVertices() = 0;
    virtual VkDeviceSize GetMemorySize(uint uiIndex) = 0;
    virtual void FillMemory(void* pMemory, uint uiIndex) = 0;
    virtual uint GetVertexCount(uint uiIndex) = 0;

    virtual VkImageView& GetImageView(uint uiIndex) = 0;
    virtual VkSampler& GetImageSampler(uint uiIndex) = 0;

    void SetParent(cInvalidatable* pParent);
    void Invalidate() override;

    void SetRotation(const glm::vec2 poRotation);
    void SetPosition(const glm::vec2 poPosition);
    void SetScale(const glm::vec2 poScale);
    void RotateLeft(float fAngleDiff);
    void RotateRight(float fAngleDiff);

    virtual glm::mat4 GetMatrix(cWindow* pWindow, uint uiIndex);
    virtual glm::mat4 GetRawMatrix();
};

cUIElement::~cUIElement()
{
}

void cUIElement::SetFocusHandler(iFocusHandler* pFocusHandler)
{
    ppFocusHandler = pFocusHandler;
}

void cUIElement::SetParent(cInvalidatable* pParent)
{
    assert(pParent != nullptr);
    ppParent = pParent;
}

void cUIElement::Invalidate()
{
    cInvalidatable::Invalidate();
    if (ppParent != nullptr) ppParent->Invalidate();
}

void cUIElement::SetRotation(glm::vec2 oRotation)
{
    ptRotation.x = oRotation.x;
    ptRotation.y = oRotation.y;
}

void cUIElement::SetPosition(glm::vec2 oPosition)
{
    ptPosition.x = oPosition.x;
    ptPosition.y = oPosition.y;
}

void cUIElement::SetScale(glm::vec2 oScale)
{
    ptScale.x = oScale.x;
    ptScale.y = oScale.y;
}

void cUIElement::RotateLeft(float fAngleDiff)
{
    if (ptRotation.y >= 360.0f)
        ptRotation.y = 0;
    ptRotation.y += fAngleDiff;
}

void cUIElement::RotateRight(float fAngleDiff)
{
    if (ptRotation.y < 0.0f)
        ptRotation.y = 360.0f;
    ptRotation.y -= fAngleDiff;
}

glm::mat4 cUIElement::GetMatrix(cWindow* pWindow, uint uiIndex)
{
    glm::mat4 proj = glm::ortho(0.0f, (float) WIDTH, 0.0f, (float) HEIGHT);

    return proj * GetRawMatrix();
}

glm::mat4 cUIElement::GetRawMatrix()
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

    return oModel;
}

void cUIElement::OnPreLoad()
{

}
