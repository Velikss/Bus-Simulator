#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/OverlayWindow.hpp>
#include <vulkan/module/overlay/element/StaticElement.hpp>
#include <vulkan/module/overlay/element/TextElement.hpp>
#include <vulkan/module/overlay/OverlayRenderModule.hpp>

class cTestOverlay : public cOverlayWindow
{
private:
    iOverlayProvider* ppOverlayProvider;

protected:
    void LoadTextures(cTextureHandler* pTextureHandler) override
    {
        pmpTextures["roof"] = pTextureHandler->LoadTextureFromFile("resources/textures/roof.jpg");
        pmpTextures["grey"] = pTextureHandler->LoadTextureFromFile("resources/textures/uvtemplate.bmp");
    }

    void ConstructElements(cLogicalDevice* pLogicalDevice) override
    {
        pmpOverlay["test"] = new cStaticElement({300, 300}, pmpTextures["grey"], pLogicalDevice);
        pmpOverlay["test"]->SetPosition(glm::vec2(500, 500));
        pmpOverlay["test1"] = new cStaticElement({100, 100}, pmpTextures["roof"], pLogicalDevice);
        pmpOverlay["test1"]->SetPosition(glm::vec2(500, 800));

        pmpOverlay["test2"] = new cTextElement(pLogicalDevice);
        pmpOverlay["test2"]->SetPosition(glm::vec2(500, 500));
        dynamic_cast<cTextElement*>(pmpOverlay["test2"])->SetFont(20, cOverlayRenderModule::FONT, glm::vec3(1, 1, 0));
        dynamic_cast<cTextElement*>(pmpOverlay["test2"])->UpdateText("Wooooo!");
    }

public:
    cTestOverlay(iOverlayProvider* pOverlayProvider) : ppOverlayProvider(pOverlayProvider)
    {}

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
            }
        }
    }

    void HandleScroll(double dOffsetX, double dOffsetY) override
    {
    }

    void HandleCharacter(char cCharacter) override
    {
    }
};
