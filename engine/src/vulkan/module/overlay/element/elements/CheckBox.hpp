#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/element/elements/CompoundElement.hpp>
#include <vulkan/module/overlay/element/elements/SimpleButton.hpp>

class cCheckBox : public cCompoundElement, public iInputHandler
{
private:
    cSimpleButton* ppButton = nullptr;
    cStaticElement* ppCheckElement = nullptr;

    bool pbChecked = false;

public:
    cCheckBox(const tElementInfo& tSize, cTexture* pBackground, cTexture* pCheckMark);

    void SetChecked(bool bChecked);
    bool IsChecked();

    void HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction) override;

private:
    void UpdateChecked();
};

cCheckBox::cCheckBox(const tElementInfo& tSize, cTexture* pBackground, cTexture* pCheckMark)
{
    ppButton = new cSimpleButton(tSize, pBackground);
    ppCheckElement = new cStaticElement(tSize, pCheckMark);
    ppCheckElement->SetScale(glm::vec2(0));

    papChildren.push_back(ppButton);
    papChildren.push_back(ppCheckElement);
}

void cCheckBox::HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction)
{
    if (iAction == GLFW_PRESS && uiButton == GLFW_MOUSE_BUTTON_LEFT)
    {
        glm::vec2 tClick = glm::inverse(cUIElement::GetRawMatrix()) * glm::vec4(dXPos, dYPos, 0, 1);
        if (ppButton->DidClick(tClick))
        {
            pbChecked = !pbChecked;
            UpdateChecked();
        }
    }
}

void cCheckBox::SetChecked(bool bChecked)
{
    pbChecked = bChecked;
    UpdateChecked();
}

bool cCheckBox::IsChecked()
{
    return pbChecked;
}

void cCheckBox::UpdateChecked()
{
    ppCheckElement->SetScale(glm::vec2(pbChecked ? 1 : 0));
}
