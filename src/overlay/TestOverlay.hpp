#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/OverlayWindow.hpp>
#include <vulkan/module/overlay/element/StaticElement.hpp>
#include <vulkan/module/overlay/element/TextElement.hpp>
#include <vulkan/module/overlay/OverlayRenderModule.hpp>
#include <vulkan/module/overlay/element/elements/SimpleButton.hpp>

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
        pmpOverlay["test"] = new cStaticElement({300, 300}, pmpTextures["grey"]);
        pmpOverlay["test"]->SetPosition(glm::vec2(500, 500));

        pmpOverlay["test1"] = pButton = new cSimpleButton({200, 200}, pmpTextures["roof"]);
        pButton->SetPosition(glm::vec2(500, 900));

        pmpOverlay["test2"] = pText = new cTextElement();
        pText->SetPosition(glm::vec2(500, 500));
        pText->SetFont(20, cOverlayRenderModule::FONT, glm::vec3(1, 1, 0));
        pText->UpdateText("Wooooo!");
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

    void HandleMouse(uint uiDeltaX, uint uiDeltaY) override
    {
    }

    void HandleKey(uint uiKeyCode, uint uiAction) override
    {
        if (uiAction == GLFW_PRESS)
        {
            switch (uiKeyCode)
            {
                case GLFW_KEY_ESCAPE:
                    ppOverlayProvider->DeactivateOverlayWindow();
                    break;
                case GLFW_KEY_Q:
                    Quit();
                    break;
                case GLFW_KEY_E:
                    pText->UpdateText("TEST");
                    break;
                case GLFW_KEY_X:
                    pmpOverlay["test"]->SetScale(glm::vec2(0));
                    break;
                case GLFW_KEY_C:
                    pmpOverlay["test"]->SetScale(glm::vec2(1));
                    break;
            }
        }
    }

    void HandleScroll(double dOffsetX, double dOffsetY) override
    {
    }

    void HandleCharacter(char cCharacter) override
    {
    }

    void HandleMouseButton(uint uiButton, double dXPos, double dYPos) override
    {
        glm::vec2 tPos(dXPos, dYPos);
        if (pButton->DidClick(tPos))
        {
            pText->UpdateText("Click!");
            uiClickTimer = 60;
        }
    }
};
