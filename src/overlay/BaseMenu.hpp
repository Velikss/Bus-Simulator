#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/OverlayWindow.hpp>
#include <vulkan/module/overlay/element/StaticElement.hpp>
#include <vulkan/module/overlay/element/TextElement.hpp>
#include <vulkan/module/overlay/OverlayRenderModule.hpp>
#include <vulkan/module/overlay/element/elements/cButton.hpp>

class cBaseMenu : public cOverlayWindow
{
protected:
    iGameManager* ppGameManager;

protected:
    virtual void LoadTextures(cTextureHandler* pTextureHandler) override
    {
        pmpTextures["buttonTexture"] = pTextureHandler->LoadTextureFromFile("resources/textures/button.png");
        pmpTextures["background"] = pTextureHandler->LoadTextureFromFile("resources/textures/background.png");
    }

    virtual void ConstructElements() override
    {
        cStaticElement* oPanel = new cStaticElement({(uint)((double)cWindow::puiWidth * 0.85), (uint)((double)cWindow::puiHeight * 0.85)}, pmpTextures["background"]);
        oPanel->Center();
        pmpOverlay.push_back({"Background", oPanel});

        cButton* oExit = new cButton({40, 40}, 0, pmpTextures["buttonTexture"],
                                     cOverlayRenderModule::FONT, 5,
                                     glm::vec3(0, 0, 0));
        oExit->SetLabel("X");
        oExit->CenterHorizontal();
        oExit->AddX(780);
        oExit->AddY(100);
        oExit->ppaCallbacks.push_back([&] (cButton* poSender) -> void {
            ppGameManager->DeactivateOverlayWindow();
        });
        pmpOverlay.push_back({"oExit", oExit});
    }
public:
    cBaseMenu(iGameManager* pOverlayProvider) : ppGameManager(pOverlayProvider)
    {
    }

    void Tick() override
    {
    }

    bool ShouldHandleInput() override
    {
        return true;
    }

    void OnOpen() override
    {
        cWindow::SetMouseLocked(false);
    }

    void OnClose() override
    {
        cWindow::SetMouseLocked(true);
    }
};
