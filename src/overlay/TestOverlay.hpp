#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/OverlayWindow.hpp>
#include <vulkan/module/overlay/element/StaticElement.hpp>
#include <vulkan/module/overlay/element/TextElement.hpp>
#include <vulkan/module/overlay/OverlayRenderModule.hpp>
#include <vulkan/module/overlay/element/elements/SimpleButton.hpp>
#include <vulkan/module/overlay/element/elements/TextBoxElement.hpp>
#include <vulkan/module/overlay/element/elements/PasswordTextBox.hpp>
#include <vulkan/module/overlay/element/elements/TabsElement.hpp>
#include <vulkan/module/overlay/element/elements/ComboBox.hpp>

class cTestOverlay : public cOverlayWindow
{
private:
    iOverlayProvider* ppOverlayProvider;

    cClickableElement* pButton;
    cTextElement* pText;

    uint uiClickTimer = 0;

protected:
    void LoadTextures(cTextureHandler* pTextureHandler) override
    {
        pmpTextures["roof"] = pTextureHandler->LoadTextureFromFile("resources/textures/roof.jpg");
        pmpTextures["grey"] = pTextureHandler->LoadTextureFromFile("resources/textures/uvtemplate.bmp");
    }

    void ConstructElements() override
    {
        tFontInfo tTabsFont = {cOverlayRenderModule::FONT, 13, glm::vec3(1, 1, 0)};
        tFontInfo tComboFont = {cOverlayRenderModule::FONT, 10, glm::vec3(1, 1, 0)};

        cTabElement* pFirstTab = new cTabElement();
        /*pFirstTab->pmpElements["textbox1"] = new cTextBoxElement({400, 120}, 2, pmpTextures["roof"],
                                                                 cOverlayRenderModule::FONT, 13,
                                                                 glm::vec3(1, 1, 0));
        pFirstTab->pmpElements["textbox1"]->SetPosition(glm::vec2(0, 500));*/

        cComboBox* pComboBox = new cComboBox({200, 50}, tComboFont,
                                             pmpTextures["roof"], pmpTextures["grey"]);
        pComboBox->SetPosition({200, 600});
        pComboBox->AddOption("Test 1");
        pComboBox->AddOption("Test 2");
        pComboBox->AddOption("Test 3");
        pFirstTab->pmpElements["combobox"] = pComboBox;

        cTabElement* pSecondTab = new cTabElement();
        pSecondTab->pmpElements["background"] = new cStaticElement({1920, 1080}, pmpTextures["grey"]);
        pSecondTab->pmpElements["background"]->SetPosition({0, 100});

        pSecondTab->pmpElements["textbox2"] = new cPasswordTextBox({300, 80}, 2, pmpTextures["roof"],
                                                                   cOverlayRenderModule::FONT, 13,
                                                                   glm::vec3(1, 1, 0));
        pSecondTab->pmpElements["textbox2"]->SetPosition(glm::vec2(500, 500));

        cTabsElement * pTabs = new cTabsElement({1000, 100}, pmpTextures["roof"],
                                                tTabsFont, pmpTextures["grey"], this);
        pTabs->pmpTabs["Tab 1"] = pFirstTab;
        pTabs->pmpTabs["Tab 2"] = pSecondTab;

        pmpOverlay["tabs"] = pTabs;
    }

public:
    cTestOverlay(iOverlayProvider* pOverlayProvider) : ppOverlayProvider(pOverlayProvider)
    {}

    void Tick() override
    {
        if (uiClickTimer > 0)
        {
            uiClickTimer--;
            if (uiClickTimer == 0)
            {
                pText->UpdateText("");
            }
        }
    }

    bool ShouldHandleInput() override
    {
        return true;
    }
};
