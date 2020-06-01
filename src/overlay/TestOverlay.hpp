#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/OverlayWindow.hpp>
#include <vulkan/module/overlay/element/StaticElement.hpp>
#include <vulkan/module/overlay/element/TextElement.hpp>
#include <vulkan/module/overlay/OverlayRenderModule.hpp>
#include <vulkan/module/overlay/element/elements/SimpleButton.hpp>
#include <vulkan/module/overlay/element/elements/TextBoxElement.hpp>

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
        /*pmpOverlay["test"] = new cStaticElement({300, 300}, pmpTextures["grey"]);
        pmpOverlay["test"]->SetPosition(glm::vec2(500, 500));

        pmpOverlay["test1"] = pButton = new cSimpleButton({200, 200}, pmpTextures["roof"]);
        pButton->SetPosition(glm::vec2(500, 900));

        pmpOverlay["test2"] = pText = new cTextElement();
        pText->SetPosition(glm::vec2(500, 500));
        pText->SetFont(20, cOverlayRenderModule::FONT, glm::vec3(1, 1, 0));
        pText->UpdateText("Wooooo!");*/

        pmpOverlay["textbox1"] = new cTextBoxElement({300, 80}, pmpTextures["roof"],
                                                     cOverlayRenderModule::FONT, 13,
                                                     glm::vec3(1, 1, 0));
        pmpOverlay["textbox1"]->SetPosition(glm::vec2(0, 500));

        pmpOverlay["textbox2"] = new cTextBoxElement({300, 80}, pmpTextures["roof"],
                                                     cOverlayRenderModule::FONT, 13,
                                                     glm::vec3(1, 1, 0));
        pmpOverlay["textbox2"]->SetPosition(glm::vec2(500, 500));
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
