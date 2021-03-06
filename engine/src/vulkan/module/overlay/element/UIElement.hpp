#pragma once

#include <pch.hpp>
#include <vulkan/util/Invalidatable.hpp>
#include <vulkan/module/overlay/FocusHandler.hpp>

struct tElementInfo
{
    uint uiWidth;
    uint uiHeight;
};

class cUIElement : public cInvalidatable, public cFocussable
{
protected:
    iFocusHandler* ppFocusHandler = nullptr;

    cInvalidatable* ppParent = nullptr;

    glm::vec2 ptRotation = glm::vec2(0.0f, 0.0f);
    glm::vec2 ptPosition = glm::vec2(0.0f, 0.0f);
    glm::vec2 ptScale = glm::vec2(1.0f, 1.0f);
    tElementInfo ptInfo = {0,0};

public:
    virtual ~cUIElement();
    virtual void SetFocusHandler(iFocusHandler* pFocusHandler);

    virtual uint GetChildCount() = 0;
    virtual bool IsTextElement(uint uiIndex) = 0;
    virtual glm::vec3 GetColor(uint uiIndex) = 0;
    virtual tRectangle GetScreenEstate();

    virtual void OnPreLoad();
    virtual void OnLoadVertices() = 0;
    virtual VkDeviceSize GetMemorySize(uint uiIndex) = 0;
    virtual void InitializeMemory(void* pMemory, uint uiIndex) = 0;
    virtual uint GetVertexCount(uint uiIndex) = 0;

    virtual VkImageView& GetImageView(uint uiIndex) = 0;
    virtual VkSampler& GetImageSampler(uint uiIndex) = 0;

    void SetParent(cInvalidatable* pParent);
    void Invalidate() override;

    void SetRotation(const glm::vec2 poRotation);
    void SetPosition(const glm::vec2 poPosition);
    void SetScale(const glm::vec2 poScale);
    void AddX(float fAddX);
    void RemoveX(float fRemX);
    void AddY(float fAddY);
    void RemoveY(float fRemY);
    void RotateLeft(float fAngleDiff);
    void RotateRight(float fAngleDiff);
    glm::vec2 GetPosition();
    glm::vec2 GetScale();
    tElementInfo GetSize();

    void Center();
    void CenterHorizontal();
    void CenterVertical();
    void AlignBottom();

    virtual glm::mat4 GetMatrix(cWindow* pWindow, uint uiIndex);
    virtual glm::mat4 GetRawMatrix();

    void Hide();
    void Show();
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
    glm::mat4 proj = glm::ortho(0.0f, (float) cWindow::puiWidth, 0.0f, (float) cWindow::puiHeight);

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

glm::vec2 cUIElement::GetPosition()
{
    return ptPosition;
}

glm::vec2 cUIElement::GetScale()
{
    return ptScale;
}

void cUIElement::AddX(float fAddX)
{
    ptPosition.x += fAddX;
}

void cUIElement::AddY(float fAddY)
{
    ptPosition.y += fAddY;
}

void cUIElement::OnPreLoad()
{

}

void cUIElement::CenterVertical()
{
    SetPosition({ptPosition.x, (((float)cWindow::puiHeight - ptInfo.uiHeight) / 2)});
}

void cUIElement::CenterHorizontal()
{
    SetPosition({(((float)cWindow::puiWidth - ptInfo.uiWidth) / 2), ptPosition.y});
}

void cUIElement::Center()
{
    CenterVertical();
    CenterHorizontal();
}

tElementInfo cUIElement::GetSize()
{
    auto oScale = ptScale;
    auto oSize = ptInfo;
    oSize.uiWidth *= ptScale.x;
    oSize.uiHeight *= ptScale.y;
    return oSize;
}

void cUIElement::RemoveX(float fRemX)
{
    ptPosition.x -= fRemX;
}

void cUIElement::RemoveY(float fRemY)
{
    ptPosition.y -= fRemY;
}

void cUIElement::Hide()
{
    SetScale({0, 0});
}

void cUIElement::Show()
{
    SetScale({1, 1});
}

void cUIElement::AlignBottom()
{
    SetPosition({ptPosition.x, ((float)cWindow::puiHeight - ptInfo.uiHeight)});
}

tRectangle cUIElement::GetScreenEstate()
{
    auto oPosition = GetPosition();
    auto oSize = GetSize();
    tRectangle oWindowCollision = {};
    oWindowCollision.aVertices[0] = oPosition;
    oPosition.y += oSize.uiHeight;
    oWindowCollision.aVertices[1] = oPosition;
    oPosition.x += oSize.uiWidth;
    oWindowCollision.aVertices[2] = oPosition;
    oPosition.y -= oSize.uiHeight;
    oWindowCollision.aVertices[3] = oPosition;
    return oWindowCollision;
}
