#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/OverlayWindow.hpp>
#include <vulkan/module/overlay/element/TextElement.hpp>
#include <vulkan/module/overlay/OverlayRenderModule.hpp>

class cLoadingOverlay : public cOverlayWindow
{
protected:
    void LoadTextures(cTextureHandler* pTextureHandler)
    {
    }

    void ConstructElements() override
    {
        cTextElement* pText = new cTextElement();
        pText->SetFont(16, cOverlayRenderModule::FONT, glm::vec3(1, 1, 1));
        pText->UpdateText("Loading...");
        pText->Center();
        pmpOverlay.push_back({"text", pText});
    }

public:
    void Tick()
    {
    }

    bool ShouldHandleInput() override
    {
        return false;
    }
};
