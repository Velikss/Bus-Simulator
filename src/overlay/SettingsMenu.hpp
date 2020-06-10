#pragma once

#include <pch.hpp>
#include <overlay/BaseMenu.hpp>
#include <vulkan/module/overlay/element/elements/ComboBox.hpp>
#include <vulkan/util/Settings.hpp>
#include <vulkan/util/EngineLog.hpp>
#include <vulkan/module/overlay/element/elements/ValueSelector.hpp>
#include <vulkan/module/lighting/LightingUniformHandler.hpp>

class cSettingsMenu : public cBaseMenu
{
private:
    cButton* ppApplyButton;
    cComboBox* ppResolution;
    cValueSelector* ppGamma;
    cComboBox* ppLightMode;
    cCheckBox* ppFullscreen;
    cComboBox* ppAntiAliasing;
    cButton* ppQuitButton;

    bool pbResolutionChanged = false;
    bool pbGammaChanged = false;
    bool pbLightModeChanged = false;
    bool pbAntiAliasingChanged = false;

public:
    cSettingsMenu(iGameManager* pOverlayProvider);
    void HandleKey(uint uiKeyCode, uint uiAction) override;

protected:
    void LoadTextures(cTextureHandler* pTextureHandler) override;
    void ConstructElements() override;

private:
    void HandleAction(cUIElement* pButton);
};

cSettingsMenu::cSettingsMenu(iGameManager* pOverlayProvider) : cBaseMenu(pOverlayProvider)
{}

void cSettingsMenu::LoadTextures(cTextureHandler* pTextureHandler)
{
    cBaseMenu::LoadTextures(pTextureHandler);

    pmpTextures["foreground"] = pTextureHandler->LoadFromFile("resources/textures/grey.jpg");
    pmpTextures["unchecked"] = pTextureHandler->LoadFromFile("resources/textures/unchecked.png");
    pmpTextures["checked"] = pTextureHandler->LoadFromFile("resources/textures/checked.png");
    pmpTextures["button-left-lowres"] = pTextureHandler->LoadFromFile("resources/textures/button-left-lowres.png");
}

void cSettingsMenu::ConstructElements()
{
    cBaseMenu::ConstructElements();

    tFontInfo tNormalFont = {cOverlayRenderModule::FONT, 10, glm::vec3(0, 0, 0)};
    tFontInfo tLabelFont = {cOverlayRenderModule::FONT, 6, glm::vec3(0, 0, 0)};

    std::function<void(cUIElement*)> fHandleAction = std::bind(&cSettingsMenu::HandleAction, this,
                                                               std::placeholders::_1);

    ppResolution = new cComboBox({300, 50}, tNormalFont,
                                 pmpTextures["foreground"],
                                 pmpTextures["button-left-lowres"]);
    for (auto&[sName, tResolution] : cSettings::pmtResolutions)
    {
        ppResolution->AddOption(sName);
    }
    ppResolution->SetSelected("1920x1080");
    ppResolution->pafCallbacks.push_back(fHandleAction);
    ppResolution->Center();
    ppResolution->AddX(-750);
    ppResolution->AddY(-360);
    cTextElement* pResolutionLabel = new cTextElement();
    pResolutionLabel->SetFont(tLabelFont);
    pResolutionLabel->UpdateText("Resolution:");
    pResolutionLabel->Center();
    pResolutionLabel->AddX(-680);
    pResolutionLabel->AddY(-380);

    ppGamma = new cValueSelector({300, 50}, tNormalFont,
                                 pmpTextures["foreground"],
                                 pmpTextures["button-left-lowres"],
                                 0.5f, 2.5f, 0.1f);
    ppGamma->SetValue(cLightingUniformHandler::pfGamma);
    ppGamma->pafCallbacks.push_back(fHandleAction);
    ppGamma->Center();
    ppGamma->AddX(-750);
    ppGamma->AddY(-270);
    cTextElement* pGammaLabel = new cTextElement();
    pGammaLabel->SetFont(tLabelFont);
    pGammaLabel->UpdateText("Gamma:");
    pGammaLabel->Center();
    pGammaLabel->AddX(-695);
    pGammaLabel->AddY(-290);

    ppLightMode = new cComboBox({300, 50}, tNormalFont,
                                pmpTextures["foreground"],
                                pmpTextures["button-left-lowres"]);
    ppLightMode->AddOption("Medium");
    ppLightMode->AddOption("High");
    ppLightMode->SetSelected("High");
    ppLightMode->pafCallbacks.push_back(fHandleAction);
    ppLightMode->Center();
    ppLightMode->AddX(-750);
    ppLightMode->AddY(-180);
    cTextElement* pLightModeLabel = new cTextElement();
    pLightModeLabel->SetFont(tLabelFont);
    pLightModeLabel->UpdateText("Lighting Quality:");
    pLightModeLabel->Center();
    pLightModeLabel->AddX(-650);
    pLightModeLabel->AddY(-200);

    ppFullscreen = new cCheckBox({75, 75},
                                 pmpTextures["unchecked"],
                                 pmpTextures["checked"]);
    ppFullscreen->Center();
    ppFullscreen->AddX(-580);
    ppFullscreen->AddY(-90);
    cTextElement* pFullscreenLabel = new cTextElement();
    pFullscreenLabel->SetFont(tLabelFont);
    pFullscreenLabel->UpdateText("Fullscreen:");
    pFullscreenLabel->Center();
    pFullscreenLabel->AddX(-670);
    pFullscreenLabel->AddY(-55);

    ppAntiAliasing = new cComboBox({300, 50}, tNormalFont,
                                   pmpTextures["foreground"],
                                   pmpTextures["button-left-lowres"]);
    ppAntiAliasing->AddOption("Off");
    ppAntiAliasing->AddOption("2x");
    ppAntiAliasing->AddOption("4x");
    ppAntiAliasing->AddOption("8x");
    ppAntiAliasing->SetSelected("4x");
    ppAntiAliasing->pafCallbacks.push_back(fHandleAction);
    ppAntiAliasing->Center();
    ppAntiAliasing->AddX(-750);
    ppAntiAliasing->AddY(35);
    cTextElement* pAntiAliasingLabel = new cTextElement();
    pAntiAliasingLabel->SetFont(tLabelFont);
    pAntiAliasingLabel->UpdateText("Anti Aliasing:");
    pAntiAliasingLabel->Center();
    pAntiAliasingLabel->AddX(-670);
    pAntiAliasingLabel->AddY(15);

    ppApplyButton = new cButton({400, 75}, 0,
                                pmpTextures["button-wide"], pmpTextures["button-wide-clicked"], tNormalFont);
    ppApplyButton->SetLabel("Apply");
    ppApplyButton->ppaCallbacks.push_back(fHandleAction);
    ppApplyButton->Center();
    ppApplyButton->AddY(400);
    ppApplyButton->AddX(-500);

    ppQuitButton = new cButton({400, 75}, 0,
                               pmpTextures["button-wide"], pmpTextures["button-wide-clicked"], tNormalFont);
    ppQuitButton->SetLabel("Exit Game");
    ppQuitButton->ppaCallbacks.push_back(fHandleAction);
    ppQuitButton->Center();
    ppQuitButton->AddY(400);
    ppQuitButton->AddX(500);

    pmpOverlay.push_back({"resolution", ppResolution});
    pmpOverlay.push_back({"resolution_label", pResolutionLabel});
    pmpOverlay.push_back({"gamma", ppGamma});
    pmpOverlay.push_back({"gamma_label", pGammaLabel});
    pmpOverlay.push_back({"light_mode", ppLightMode});
    pmpOverlay.push_back({"light_mode_label", pLightModeLabel});
    pmpOverlay.push_back({"light_mode", ppFullscreen});
    pmpOverlay.push_back({"light_mode_label", pFullscreenLabel});
    pmpOverlay.push_back({"aa_mode", ppAntiAliasing});
    pmpOverlay.push_back({"aa_mode_label", pAntiAliasingLabel});
    pmpOverlay.push_back({"apply_button", ppApplyButton});
    pmpOverlay.push_back({"quit_button", ppQuitButton});

    ((cButton*) GetElement("oExit"))->ppaCallbacks.push_back([&](cButton* poSender) -> void
                                                             {
                                                                 ppGameManager->ActivateOverlayWindow("InGame");
                                                             });
}

void cSettingsMenu::HandleAction(cUIElement* pButton)
{
    ENGINE_LOG("HandleAction");
    if (pButton == ppApplyButton)
    {
        cWindow::SetFullscreen(ppFullscreen->IsChecked());
        cLightingUniformHandler::pfGamma = ppGamma->GetValue();
        cLightingUniformHandler::peLightingMode = ppLightMode->GetSelected() == "High" ? FANCY_LIGHTING
                                                                                       : FAST_LIGHTING;
        cSwapChain::peSampleCount = cSettings::pmeSampleCounts[ppAntiAliasing->GetSelected()];
        tResolution& tResolution = cSettings::pmtResolutions[ppResolution->GetSelected()];
        cWindow::SetResolution(tResolution.puiWidth, tResolution.puiHeight);
        cWindow::RequestRebuild();
    }
    else if (pButton == ppResolution)
    {
        pbResolutionChanged = true;
    }
    else if (pButton == ppGamma)
    {
        pbGammaChanged = true;
    }
    else if (pButton == ppLightMode)
    {
        pbLightModeChanged = true;
    }
    else if (pButton == ppAntiAliasing)
    {
        pbAntiAliasingChanged = true;
    }
    else if (pButton == ppQuitButton)
    {
        Quit();
    }
}

void cSettingsMenu::HandleKey(uint uiKeyCode, uint uiAction)
{
    if (uiAction == GLFW_PRESS)
    {
        switch (uiKeyCode)
        {
            case GLFW_KEY_ESCAPE:
            case GLFW_KEY_HOME:
                ppGameManager->DeactivateOverlayWindow();
                return;
            case GLFW_KEY_ENTER:
            case GLFW_KEY_SPACE:
                HandleAction(ppApplyButton);
                return;
        }
    }

    cOverlayWindow::HandleKey(uiKeyCode, uiAction);
}
