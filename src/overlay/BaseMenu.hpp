#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/OverlayWindow.hpp>
#include <vulkan/module/overlay/element/StaticElement.hpp>
#include <vulkan/module/overlay/element/TextElement.hpp>
#include <vulkan/module/overlay/OverlayRenderModule.hpp>
#include <vulkan/module/overlay/element/elements/cButton.hpp>

class cBaseMenu : public cOverlayWindow
{
private:
    iOverlayProvider* ppOverlayProvider;
protected:
    void LoadTextures(cTextureHandler* pTextureHandler)
    {
        pmpTextures["buttonTexture"] = pTextureHandler->LoadTextureFromFile("resources/textures/button.jpg");
        pmpTextures["background"] = pTextureHandler->LoadTextureFromFile("resources/textures/background.jpg");
    }

    void ConstructElements()
    {
        pmpOverlay["oPanel"] = new cStaticElement({(uint)((double)WIDTH * 0.85), (uint)((double)HEIGHT * 0.85)}, pmpTextures["background"]);
        ((cStaticElement*)(pmpOverlay["oPanel"]))->Center();

        cButton* oExit = new cButton({40, 40}, 0, pmpTextures["buttonTexture"],
                                     cOverlayRenderModule::FONT, 5,
                                     glm::vec3(0, 0, 0));
        oExit->SetLabel("X");
        oExit->CenterHorizontal();
        oExit->AddX(780);
        oExit->AddY(100);
        oExit->ppaCallbacks.push_back([&] (cButton* poSender) -> void {
            ppOverlayProvider->DeactivateOverlayWindow();
        });
        pmpOverlay["oExit"] = oExit;
    }
public:
    cBaseMenu(iOverlayProvider* pOverlayProvider) : ppOverlayProvider(pOverlayProvider)
    {
    }

    void Tick()
    {
    }

    bool ShouldHandleInput() override
    {
        return true;
    }
};
