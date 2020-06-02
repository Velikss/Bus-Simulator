#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/element/elements/CompoundElement.hpp>
#include <vulkan/module/overlay/element/elements/SimpleButton.hpp>

class cComboBox : public cCompoundElement, public iInputHandler
{
private:
    cTextElement* ppCurrentValue = nullptr;

    cSimpleButton* ppLeftButton = nullptr;
    cSimpleButton* ppRightButton = nullptr;

    std::vector<string> pasOptions;
    uint puiSelectedIndex = 0;

public:
    cComboBox(const tElementInfo& tSize, const tFontInfo& tFont,
              cTexture* pBackgroundTexture, cTexture* pButtonTexture);

    void AddOption(const string& sOption);
    void SetSelected(const string& sSelected);
    string GetSelected();

    void HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction) override;
    void OnPreLoad() override;
    void OnLoadVertices() override;

private:
    void LeftButtonClick();
    void RightButtonClick();
    void UpdateCurrentValue();
};

cComboBox::cComboBox(const tElementInfo& tSize, const tFontInfo& tFont,
                     cTexture* pBackgroundTexture, cTexture* pButtonTexture)
{
    uint uiButtonSize = tSize.uiHeight;

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

void cComboBox::AddOption(const string& sOption)
{
    pasOptions.push_back(sOption);
}

void cComboBox::OnPreLoad()
{
    assert(pasOptions.size() != 0);
}

void cComboBox::OnLoadVertices()
{
    cCompoundElement::OnLoadVertices();
    UpdateCurrentValue();
}

void cComboBox::SetSelected(const string& sSelected)
{
    uint uiIndex = 0;
    for (string& sOption : pasOptions)
    {
        if (sOption == sSelected)
        {
            puiSelectedIndex = uiIndex;
            return;
        }
        uiIndex++;
    }

    assert(false); // the value of sSelected is not a valid option
}

string cComboBox::GetSelected()
{
    return pasOptions[puiSelectedIndex];
}

void cComboBox::HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction)
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

void cComboBox::LeftButtonClick()
{
    if (puiSelectedIndex > 0)
    {
        puiSelectedIndex--;
        UpdateCurrentValue();
    }
}

void cComboBox::RightButtonClick()
{
    if (puiSelectedIndex < (pasOptions.size() - 1))
    {
        puiSelectedIndex++;
        UpdateCurrentValue();
    }
}

void cComboBox::UpdateCurrentValue()
{
    ppCurrentValue->UpdateText(pasOptions[puiSelectedIndex]);
}
