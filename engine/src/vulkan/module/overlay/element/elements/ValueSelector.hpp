#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/element/elements/CompoundElement.hpp>
#include <vulkan/module/overlay/element/elements/SimpleButton.hpp>
#include <util/Formatter.hpp>

class cValueSelector : public cCompoundElement, public iInputHandler
{
public:
    std::vector<std::function<void(cValueSelector*)>> pafCallbacks;

private:
    cTextElement* ppCurrentValue = nullptr;

    cSimpleButton* ppLeftButton = nullptr;
    cSimpleButton* ppRightButton = nullptr;

    float pfMinValue;
    float pfMaxValue;
    float pfStepSize;

    float pfSelectedValue;

public:
    cValueSelector(const tElementInfo& tSize, const tFontInfo& tFont,
                   cTexture* pBackgroundTexture, cTexture* pButtonTexture,
                   float fMinValue, float fMaxValue, float fStepSize);

    void SetValue(float fValue);
    float GetValue();

    void HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction) override;
    void OnLoadVertices() override;

private:
    void LeftButtonClick();
    void RightButtonClick();
    void UpdateCurrentValue();
    void NotifyCallbacks();
};

cValueSelector::cValueSelector(const tElementInfo& tSize, const tFontInfo& tFont,
                               cTexture* pBackgroundTexture, cTexture* pButtonTexture,
                               float fMinValue, float fMaxValue, float fStepSize)
{
    uint uiButtonSize = tSize.uiHeight;

    pfMinValue = fMinValue;
    pfMaxValue = fMaxValue;
    pfStepSize = fStepSize;

    cStaticElement* pBackground = new cStaticElement(tSize, pBackgroundTexture);
    pBackground->SetPosition({uiButtonSize, 0});
    ppCurrentValue = new cTextElement();
    ppCurrentValue->SetFont(tFont.pfFontSize, tFont.ppFont, tFont.ptFontColor);
    ppCurrentValue->SetPosition({uiButtonSize, 0});
    ppCurrentValue->SetParent(this);

    ppLeftButton = new cSimpleButton({uiButtonSize, uiButtonSize}, pButtonTexture);
    ppRightButton = new cSimpleButton({uiButtonSize, uiButtonSize}, pButtonTexture);
    ppRightButton->SetPosition({tSize.uiWidth + uiButtonSize, uiButtonSize});
    ppRightButton->SetRotation({0, 180});

    papChildren.push_back(pBackground);
    papChildren.push_back(ppCurrentValue);
    papChildren.push_back(ppRightButton);
    papChildren.push_back(ppLeftButton);
}

void cValueSelector::OnLoadVertices()
{
    cCompoundElement::OnLoadVertices();
    UpdateCurrentValue();
}

void cValueSelector::SetValue(float fValue)
{
    pfSelectedValue = fValue;
    UpdateCurrentValue();
}

float cValueSelector::GetValue()
{
    return pfSelectedValue;
}

void cValueSelector::HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction)
{
    if (iAction == GLFW_PRESS)
    {
        glm::vec2 tClick = glm::inverse(cUIElement::GetRawMatrix()) * glm::vec4(dXPos, dYPos, 0, 1);
        if (ppLeftButton->DidClick(tClick))
        {
            LeftButtonClick();
        }
        else if (ppRightButton->DidClick(tClick))
        {
            RightButtonClick();
        }
    }
}

void cValueSelector::LeftButtonClick()
{
    if (pfSelectedValue > pfMinValue)
    {
        pfSelectedValue -= pfStepSize;
        UpdateCurrentValue();
        NotifyCallbacks();
    }
}

void cValueSelector::RightButtonClick()
{
    if (pfSelectedValue < (pfMaxValue - pfStepSize))
    {
        pfSelectedValue += pfStepSize;
        UpdateCurrentValue();
        NotifyCallbacks();
    }
}

void cValueSelector::UpdateCurrentValue()
{
    ppCurrentValue->UpdateText(cFormatter() << pfSelectedValue);
}

void cValueSelector::NotifyCallbacks()
{
    for (auto& fEventHandler : pafCallbacks)
    {
        fEventHandler(this);
    }
}
