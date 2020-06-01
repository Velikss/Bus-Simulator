#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/element/elements/TextBoxElement.hpp>

class cPasswordTextBox : public cTextBoxElement
{
private:
    uint puiCharWidth = 0;
    uint puiMaxChars = 0;

public:
    cPasswordTextBox(const tElementInfo& tSize, uint uiPadding, cTexture* pTexture, cFont* ppFont, float fFontSize,
                     const glm::vec3& tTextColor);
protected:
    void UpdateText() override;
};

cPasswordTextBox::cPasswordTextBox(const tElementInfo& tSize, uint uiPadding, cTexture* pTexture,
                                   cFont* ppFont, float fFontSize, const glm::vec3& tTextColor)
        : cTextBoxElement(tSize, uiPadding, pTexture, ppFont, fFontSize, tTextColor)
{
    puiCharWidth = cTextElement::GetTextWidth("*", ppFont, pfFontSize);
    puiMaxChars = tSize.uiWidth / puiCharWidth;
}

void cPasswordTextBox::UpdateText()
{
    psDisplayText.clear();
    uint uiLength = MIN(psText.length(), puiMaxChars);
    for (uint uiIndex = 0; uiIndex < uiLength; uiIndex++)
    {
        psDisplayText += "*";
    }
    ppTextElement->UpdateText(psDisplayText);
}
