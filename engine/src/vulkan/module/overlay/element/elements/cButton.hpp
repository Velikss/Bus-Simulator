#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/element/elements/SimpleButton.hpp>
#include <vulkan/module/overlay/element/elements/LabeledElement.hpp>

class cButton : public cLabeledElement, public iInputHandler
{
protected:
    cClickableElement* ppClickable = nullptr;

    virtual void HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction) override;
public:
    std::vector<std::function<void(cButton*)>> ppaCallbacks;
    cStaticElement* ppForeground = nullptr;
    cButton(tElementInfo tSize, uint uiPadding, cTexture* pTexture, cTexture* pTextureWhenClicked, cFont* pFont, float fFontSize, glm::vec3 tTextColor);
    cButton(tElementInfo tSize, uint uiPadding, cTexture* pTexture, cTexture* pTextureWhenClicked, const tFontInfo& tFont);
};

cButton::cButton(tElementInfo tSize, uint uiPadding, cTexture* pTexture, cTexture* pTextureWhenClicked, cFont* pFont, float fFontSize, glm::vec3 tTextColor)
                                 : cLabeledElement(tSize, uiPadding, pTexture, pFont, fFontSize, tTextColor)
{
    ppClickable = new cSimpleButton(tSize, pTexture);
    ppForeground = new cStaticElement(tSize, pTextureWhenClicked);
    ppForeground->Hide();
    AddChild(ppClickable);
    AddChild(ppForeground);
    AddLabelLayer();
}

cButton::cButton(tElementInfo tSize, uint uiPadding, cTexture* pTexture, cTexture* pTextureWhenClicked, const tFontInfo& tFont)
        : cButton(tSize, uiPadding, pTexture, pTextureWhenClicked, tFont.ppFont, tFont.pfFontSize, tFont.ptFontColor)
{
}

void cButton::HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction)
{
    glm::vec2 tClick = glm::inverse(cUIElement::GetRawMatrix()) * glm::vec4(dXPos, dYPos, 0, 1);
    if (ppClickable->DidClick(tClick) && iAction == GLFW_PRESS)
    {
        ppForeground->Show();
        ppFocusHandler->SetFocussedElement(this);
    }

    if(ppClickable->DidClick(tClick) && iAction == GLFW_RELEASE) {
        for(auto& oEventHandler : ppaCallbacks)
            oEventHandler(this);
        ppForeground->Hide();
    }
}
