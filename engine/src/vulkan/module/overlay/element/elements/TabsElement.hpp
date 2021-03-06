#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/OverlayWindow.hpp>
#include "CompoundElement.hpp"
#include "SimpleButton.hpp"

class cTabElement
{
public:
    std::map<string, cUIElement*> pmpElements;

    void SetVisible(bool bVisible);
};

void cTabElement::SetVisible(bool bVisible)
{
    for (auto&[sName, pElement] : pmpElements)
    {
        if(bVisible)
            pElement->Show();
        else
            pElement->Hide();
    }
}

class cTabsElement : public cCompoundElement, public iInputHandler
{
public:
    std::map<string, cTabElement*> pmpTabs;
    string psSelectedTab;

private:
    std::map<string, cSimpleButton*> pmpTabButtons;
    cStaticElement* ppBackground = nullptr;

    cOverlayWindow* ppParent = nullptr;

    tFontInfo ptFontInfo;
    cTexture* ppTabTexture = nullptr;
    uint puiHeight = 0;

public:
    cTabsElement(const tElementInfo& tInfo, cTexture* pBackgroundTexture,
                 const tFontInfo& tFont, cTexture* pTabTexture, cOverlayWindow* pParent);

    void OnLoadVertices() override;
    void HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction) override;
    void OnPreLoad() override;
};

cTabsElement::cTabsElement(const tElementInfo& tInfo, cTexture* pBackgroundTexture,
                           const tFontInfo& tFont, cTexture* pTabTexture, cOverlayWindow* pParent)
{
    ptFontInfo = tFont;
    ppTabTexture = pTabTexture;
    puiHeight = tInfo.uiHeight;
    ppParent = pParent;

    ppBackground = new cStaticElement(tInfo, pBackgroundTexture);
}

void cTabsElement::OnPreLoad()
{
    for (auto&[sName, pTab] : pmpTabs)
    {
        for (auto&[sChildName, pElement] : pTab->pmpElements)
        {
            ppParent->pmpOverlay.push_back({sName + sChildName, pElement});
            pElement->SetScale(glm::vec2(0));
        }
    }

    cUIElement::OnPreLoad();
}

void cTabsElement::OnLoadVertices()
{
    AddChild(ppBackground);

    uint uiOffset = 0;
    for (auto&[sName, pTab] : pmpTabs)
    {
        uint uiWidth = cTextElement::GetTextWidth(sName, ptFontInfo.ppFont, ptFontInfo.pfFontSize);
        glm::vec2 tPosition(uiOffset, 0);
        pmpTabButtons[sName] = new cSimpleButton({uiWidth, puiHeight}, ppTabTexture);
        pmpTabButtons[sName]->SetPosition(tPosition);

        cTextElement* pLabel = new cTextElement();
        pLabel->SetFont(ptFontInfo.pfFontSize, ptFontInfo.ppFont, ptFontInfo.ptFontColor);
        pLabel->SetPosition(tPosition);
        pLabel->UpdateText(sName);

        AddChild(pmpTabButtons[sName]);
        AddChild(pLabel);

        uiOffset += (uiWidth + 10);
    }

    cCompoundElement::OnLoadVertices();
}

void cTabsElement::HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction)
{
    glm::vec2 tClick = glm::inverse(cUIElement::GetRawMatrix()) * glm::vec4(dXPos, dYPos, 0, 1);
    for (auto&[sName, pButton] : pmpTabButtons)
    {
        if (pButton->DidClick(tClick))
        {
            if (!psSelectedTab.empty())
            {
                pmpTabs[psSelectedTab]->SetVisible(false);
            }
            psSelectedTab = sName;
            pmpTabs[psSelectedTab]->SetVisible(true);
            return;
        }
    }
}
