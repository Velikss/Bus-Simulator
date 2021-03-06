#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/OverlayWindow.hpp>
#include <vulkan/module/overlay/element/StaticElement.hpp>
#include <vulkan/module/overlay/element/TextElement.hpp>
#include <vulkan/module/overlay/OverlayRenderModule.hpp>
#include <vulkan/module/overlay/element/elements/Button.hpp>

class cBaseMenu : public cOverlayWindow
{
protected:
    iGameManager* ppGameManager;

protected:
    virtual void LoadTextures(cTextureHandler* pTextureHandler) override
    {
        pmpTextures["button"] = pTextureHandler->LoadFromFile("resources/textures/button.png");
        pmpTextures["button-wide"] = pTextureHandler->LoadFromFile("resources/textures/button-wide.png");
        pmpTextures["button-exit"] = pTextureHandler->LoadFromFile("resources/textures/button-exit.png");
        pmpTextures["button-left"] = pTextureHandler->LoadFromFile("resources/textures/button-left.png");
        pmpTextures["button-right"] = pTextureHandler->LoadFromFile("resources/textures/button-right.png");
        pmpTextures["button-clicked"] = pTextureHandler->LoadFromFile("resources/textures/button-clicked.png");
        pmpTextures["button-wide-clicked"] = pTextureHandler->LoadFromFile("resources/textures/button-wide-clicked.png");
        pmpTextures["button-exit-clicked"] = pTextureHandler->LoadFromFile("resources/textures/button-exit-clicked.png");
        pmpTextures["button-left-clicked"] = pTextureHandler->LoadFromFile("resources/textures/button-left-clicked.png");
        pmpTextures["button-right-clicked"] = pTextureHandler->LoadFromFile("resources/textures/button-right-clicked.png");
        pmpTextures["background"] = pTextureHandler->LoadFromFile("resources/textures/background.png");
    }

    virtual void ConstructElements() override
    {
        cStaticElement* oPanel = new cStaticElement({(uint)((double)cWindow::puiWidth * 0.85), (uint)((double)cWindow::puiHeight * 0.85)}, pmpTextures["background"]);
        oPanel->Center();
        pmpOverlay.push_back({"Background", oPanel});

        cButton* oExit = new cButton({40, 40}, 0, pmpTextures["button-exit"], pmpTextures["button-exit-clicked"],
                                     cOverlayRenderModule::FONT, 5,
                                     glm::vec3(0, 0, 0));
        oExit->CenterHorizontal();
        oExit->AddX(740);
        oExit->AddY(120);
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
