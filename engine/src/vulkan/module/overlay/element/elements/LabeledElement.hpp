#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/element/elements/CompoundElement.hpp>
#include <vulkan/module/overlay/element/elements/ClickableElement.hpp>
#include <vulkan/module/overlay/element/elements/SimpleButton.hpp>

enum class eAllignment
{
    LEFT, CENTER
};

class cLabeledElement : public cCompoundElement
{
protected:
    cTextElement* ppTextElement = nullptr;

    cFont* ppFont = nullptr;
    float pfFontSize = 8.0f;
    uint puiBoxWidth = 0;

    string psText = "";
    string psDisplayText = "";
    uint puiStartChar = 0;

    bool pbEnabled = true;
    eAllignment peTextAllignment = eAllignment::CENTER;
public:
    cLabeledElement(tElementInfo tSize, uint uiPadding, cTexture* pTexture,
            cFont* ppFont, float fFontSize, glm::vec3 tTextColor);

    void SetLabel(const string& sLabel);
    void Enable();
    void Disable();
    void SetAllignment(eAllignment eAllign);
protected:
    virtual void UpdateText();
    void ReAllign();
    void AddLabelLayer();
    uint GetTextWidth(uint uiStartChar);
};

cLabeledElement::cLabeledElement(tElementInfo tSize, uint uiPadding, cTexture* pTexture,
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
}

void cLabeledElement::UpdateText()
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

    ReAllign();
}

uint cLabeledElement::GetTextWidth(uint uiStartChar)
{
    return cTextElement::GetTextWidth(psText.substr(uiStartChar), ppFont, pfFontSize);
}

void cLabeledElement::SetLabel(const string& sLabel)
{
    this->psText = sLabel;
    UpdateText();
}

void cLabeledElement::Enable()
{
    this->pbEnabled = true;
}

void cLabeledElement::Disable()
{
    this->pbEnabled = false;
}

void cLabeledElement::ReAllign()
{
    if (peTextAllignment == eAllignment::CENTER)
    {
        ppTextElement->SetPosition({(((float)ptInfo.uiWidth - cTextElement::GetTextWidth(psText, ppFont, pfFontSize)) / 2), ppTextElement->GetPosition().y});
    }
}

void cLabeledElement::SetAllignment(eAllignment eAllign)
{
    this->peTextAllignment = eAllign;
    ReAllign();
}

void cLabeledElement::AddLabelLayer()
{
    AddChild(ppTextElement);
}
