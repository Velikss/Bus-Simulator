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

    bool pbResolutionChanged = false;
    bool pbGammaChanged = false;
    bool pbLightModeChanged = false;

public:
    cSettingsMenu(iGameManager* pOverlayProvider);

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

    pmpTextures["foreground"] = pTextureHandler->LoadTextureFromFile("resources/textures/grey.jpg");
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
                                 pmpTextures["buttonTexture"]);
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
                                 pmpTextures["buttonTexture"],
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
                                pmpTextures["buttonTexture"]);
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

    ppApplyButton = new cButton({400, 75}, 0,
                                pmpTextures["buttonTexture"], tNormalFont);
    ppApplyButton->SetLabel("Apply");
    ppApplyButton->ppaCallbacks.push_back(fHandleAction);
    ppApplyButton->Center();
    ppApplyButton->AddY(400);

    pmpOverlay.push_back({"resolution", ppResolution});
    pmpOverlay.push_back({"resolution_label", pResolutionLabel});
    pmpOverlay.push_back({"gamma", ppGamma});
    pmpOverlay.push_back({"gamma_label", pGammaLabel});
    pmpOverlay.push_back({"light_mode", ppLightMode});
    pmpOverlay.push_back({"light_mode_label", pLightModeLabel});
    pmpOverlay.push_back({"apply_button", ppApplyButton});
}

void cSettingsMenu::HandleAction(cUIElement* pButton)
{
    ENGINE_LOG("HandleAction");
    if (pButton == ppApplyButton)
    {
        if (pbResolutionChanged)
        {
            tResolution& tResolution = cSettings::pmtResolutions[ppResolution->GetSelected()];
            cWindow::SetResolution(tResolution.puiWidth, tResolution.puiHeight);
        }
        if (pbGammaChanged)
        {
            cLightingUniformHandler::pfGamma = ppGamma->GetValue();
        }
        if (pbLightModeChanged)
        {
            cLightingUniformHandler::peLightingMode = ppLightMode->GetSelected() == "High" ? FANCY_LIGHTING
                                                                                           : FAST_LIGHTING;
        }
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
}
