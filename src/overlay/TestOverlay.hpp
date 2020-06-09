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
#include <vulkan/module/overlay/element/elements/CheckBox.hpp>

class cTestOverlay : public cOverlayWindow
{
private:
    iGameManager* ppOverlayProvider;

    cClickableElement* pButton;
    cTextElement* pText;

    cComboBox* ppResolutionBox;
    string resolution = "1920x1080";

    cCheckBox* ppFullscreenBox;
    bool fullscreen = false;

    uint uiClickTimer = 0;

protected:
    void LoadTextures(cTextureHandler* pTextureHandler) override
    {
        pmpTextures["roof"] = pTextureHandler->LoadFromFile("resources/textures/roof.jpg");
        pmpTextures["grey"] = pTextureHandler->LoadFromFile("resources/textures/uvtemplate.bmp");
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

        ppResolutionBox = new cComboBox({300, 50}, tComboFont,
                                        pmpTextures["roof"], pmpTextures["grey"]);
        ppResolutionBox->SetPosition({200, 600});
        ppResolutionBox->AddOption("1080x720");
        ppResolutionBox->AddOption("1920x1080");
        ppResolutionBox->AddOption("2560x1440");
        ppResolutionBox->SetSelected("1920x1080");
        pFirstTab->pmpElements["combobox"] = ppResolutionBox;

        ppFullscreenBox = new cCheckBox({50, 50},
                                             pmpTextures["roof"], pmpTextures["grey"]);
        ppFullscreenBox->SetPosition({500, 500});
        pFirstTab->pmpElements["checkbox"] = ppFullscreenBox;

        cTabElement* pSecondTab = new cTabElement();
        pSecondTab->pmpElements["background"] = new cStaticElement({1920, 1080}, pmpTextures["grey"]);
        pSecondTab->pmpElements["background"]->SetPosition({0, 100});

        pSecondTab->pmpElements["textbox2"] = new cPasswordTextBox({300, 80}, 2, pmpTextures["roof"],
                                                                   cOverlayRenderModule::FONT, 13,
                                                                   glm::vec3(1, 1, 0));
        pSecondTab->pmpElements["textbox2"]->SetPosition(glm::vec2(500, 500));

        cTabsElement* pTabs = new cTabsElement({1000, 100}, pmpTextures["roof"],
                                               tTabsFont, pmpTextures["grey"], this);
        pTabs->pmpTabs["Tab 1"] = pFirstTab;
        pTabs->pmpTabs["Tab 2"] = pSecondTab;

        pmpOverlay.push_back({"tabs", pTabs});
    }

public:
    cTestOverlay(iGameManager* pOverlayProvider) : ppOverlayProvider(pOverlayProvider)
    {}

    void Tick() override
    {
        if (resolution != ppResolutionBox->GetSelected())
        {
            resolution = ppResolutionBox->GetSelected();
            if (resolution == "1080x720")
            {
                cWindow::SetResolution(1080, 720);
            }
            else if (resolution == "1920x1080")
            {
                cWindow::SetResolution(1920, 1080);
            }
            else if (resolution == "2560x1440")
            {
                cWindow::SetResolution(2560, 1440);
            }
        }

        if (fullscreen != ppFullscreenBox->IsChecked())
        {
            fullscreen = ppFullscreenBox->IsChecked();
            cWindow::SetFullscreen(fullscreen);
        }
    }

    bool ShouldHandleInput() override
    {
        return true;
    }

    void HandleKey(uint uiKeyCode, uint uiAction) override
    {
        if (uiKeyCode == GLFW_KEY_HOME && uiAction == GLFW_PRESS)
        {
            ppOverlayProvider->DeactivateOverlayWindow();
        }
        else
        {
            cOverlayWindow::HandleKey(uiKeyCode, uiAction);
        }
    }
};
