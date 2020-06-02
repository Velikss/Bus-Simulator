#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/element/elements/CompoundElement.hpp>
#include <vulkan/module/overlay/element/elements/ClickableElement.hpp>
#include <vulkan/module/overlay/element/elements/SimpleButton.hpp>

class cButton : public cCompoundElement, public iInputHandler
{
protected:
    cTextElement* ppTextElement = nullptr;
    cClickableElement* ppClickable = nullptr;

    cFont* ppFont = nullptr;
    float pfFontSize = 8.0f;
    uint puiBoxWidth = 0;

    string psText = "";
    string psDisplayText = "";
    uint puiStartChar = 0;

public:
    cButton(tElementInfo tSize, uint uiPadding, cTexture* pTexture,
                    cFont* ppFont, float fFontSize, glm::vec3 tTextColor);

    void HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction) override;
    void SetLabel(const string& sLabel);
protected:
    virtual void UpdateText();
    uint GetTextWidth(uint uiStartChar);
};

cButton::cButton(tElementInfo tSize, uint uiPadding, cTexture* pTexture,
                                 cFont* pFont, float fFontSize, glm::vec3 tTextColor)
{
    ppFont = pFont;
    pfFontSize = fFontSize;
    puiBoxWidth = tSize.uiWidth;

    uint uiTextHeight = pFont->GetFontHeight(fFontSize) + (uiPadding * 2);
    uint uiOffset = (tSize.uiHeight - uiTextHeight) / 2;

    ppTextElement = new cTextElement();
    ppTextElement->SetParent(this);
    ppTextElement->SetFont(fFontSize, pFont, tTextColor);
    ppTextElement->SetPosition(glm::vec2(0, uiOffset));
    ppClickable = new cSimpleButton(tSize, pTexture);

    AddChild(ppClickable);
    AddChild(ppTextElement);
}

void cButton::HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction)
{
    glm::vec2 tClick = glm::inverse(cUIElement::GetRawMatrix()) * glm::vec4(dXPos, dYPos, 0, 1);
    if (ppClickable->DidClick(tClick))
    {
        ppFocusHandler->SetFocussedElement(this);
    }
}

void cButton::UpdateText()
{
    puiStartChar = 0;
    uint uiWidth = GetTextWidth(puiStartChar);
    while (uiWidth >= puiBoxWidth)
    {
        puiStartChar++;
        uiWidth = GetTextWidth(puiStartChar);
    }

    psDisplayText = psText.substr(puiStartChar);
    ppTextElement->UpdateText(psDisplayText);
}

uint cButton::GetTextWidth(uint uiStartChar)
{
    return cTextElement::GetTextWidth(psText.substr(uiStartChar), ppFont, pfFontSize);
}

void cButton::SetLabel(const string& sLabel)
{
    this->psText = sLabel;
    UpdateText();
}
