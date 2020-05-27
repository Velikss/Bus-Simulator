#pragma once

#include <pch.hpp>
#include <vulkan/scene/Scene.hpp>
#include "vulkan/scene/LightObject.hpp"

class cTestScene : public cScene
{
private:
    string psText = "";
    cTextElement* ppText;

protected:
    void Load(cTextureHandler* pTextureHandler, cLogicalDevice* pLogicalDevice, cAudioHandler* pAudioHandler) override
    {
        pmpTextures["uvtemplate"] = pTextureHandler->LoadTextureFromFile("resources/textures/uvtemplate.bmp");

        pmpOverlay["test"] = new cStaticElement({500, 500}, pmpTextures["uvtemplate"], pLogicalDevice);
        pmpOverlay["test"]->SetPosition(glm::vec2(350, 350));

        ppText = new cTextElement({100, 100}, nullptr, pLogicalDevice);
        pmpOverlay["text"] = ppText;
        ppText->SetPosition(glm::vec2(500, 500));
        ppText->SetFont(20, cOverlayRenderModule::FONT, glm::vec3(1, 1, 0));

        cScene::Load(pTextureHandler, pLogicalDevice, pAudioHandler);
    }

public:
    void Update() override
    {
        if (paKeys[GLFW_KEY_ESCAPE]) Quit();

        cScene::Update();
    }

    void HandleKey(uint uiKeyCode, uint uiAction) override
    {
        if (uiAction == GLFW_PRESS)
        {
            if (uiKeyCode == GLFW_KEY_BACKSPACE)
            {
                psText = psText.substr(0, psText.size() - 1);
                ppText->UpdateText(psText);
            }
        }
    }

    void HandleCharacter(char cCharacter) override
    {
        psText += cCharacter;
        ppText->UpdateText(psText);
    }
};
