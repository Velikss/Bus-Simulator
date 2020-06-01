#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/element/elements/CompoundElement.hpp>
#include <vulkan/module/overlay/element/elements/ClickableElement.hpp>
#include <vulkan/module/overlay/element/elements/SimpleButton.hpp>

class cTextBoxElement : public cCompoundElement, public iInputHandler
{
private:
    cTextElement* ppTextElement = nullptr;
    cClickableElement* ppClickable = nullptr;

    cFont* ppFont = nullptr;
    float pfFontSize = 8.0f;
    uint puiBoxWidth;

    string psText = "";
    string psDisplayText = "";
    uint puiStartChar = 0;

public:
    cTextBoxElement(tElementInfo tSize, cTexture* pTexture, cFont* ppFont, float fFontSize, glm::vec3 tTextColor);

    void HandleMouseButton(uint uiButton, double dXPos, double dYPos) override;
    void HandleCharacter(char cCharacter) override;
    void HandleKey(uint uiKeyCode, uint uiAction) override;

private:
    void UpdateText();
    uint GetTextWidth(uint uiStartChar);
};

cTextBoxElement::cTextBoxElement(tElementInfo tSize, cTexture* pTexture,
                                 cFont* pFont, float fFontSize, glm::vec3 tTextColor)
{
    ppFont = pFont;
    pfFontSize = fFontSize;
    puiBoxWidth = tSize.uiWidth;

    ppTextElement = new cTextElement();
    ppTextElement->SetParent(this);
    ppTextElement->SetFont(fFontSize, pFont, tTextColor);
    ppClickable = new cSimpleButton(tSize, pTexture);

    papChildren.push_back(ppClickable);
    papChildren.push_back(ppTextElement);
}

void cTextBoxElement::HandleMouseButton(uint uiButton, double dXPos, double dYPos)
{
    glm::vec2 tClick = glm::inverse(cUIElement::GetRawMatrix()) * glm::vec4(dXPos, dYPos, 0, 1);
    if (ppClickable->DidClick(tClick))
    {
        ppFocusHandler->SetFocussedElement(this);
    }
}

void cTextBoxElement::HandleCharacter(char cCharacter)
{
    if (ppFocusHandler->GetFocussedElement() == this)
    {
        psText += cCharacter;
        UpdateText();
    }
}

void cTextBoxElement::HandleKey(uint uiKeyCode, uint uiAction)
{
    if (ppFocusHandler->GetFocussedElement() == this && uiAction == GLFW_PRESS && uiKeyCode == GLFW_KEY_BACKSPACE)
    {
        psText = psText.substr(0, psText.size() - 1);
        UpdateText();
    }
}

void cTextBoxElement::UpdateText()
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

uint cTextBoxElement::GetTextWidth(uint uiStartChar)
{
    return cTextElement::GetTextWidth(psText.substr(uiStartChar), ppFont, pfFontSize);
}