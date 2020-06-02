#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/OverlayWindow.hpp>
#include <vulkan/module/overlay/element/StaticElement.hpp>
#include <vulkan/module/overlay/element/TextElement.hpp>
#include <vulkan/module/overlay/OverlayRenderModule.hpp>
#include <vulkan/module/overlay/element/elements/SimpleButton.hpp>
#include <vulkan/module/overlay/element/elements/TextBoxElement.hpp>
#include <vulkan/module/overlay/element/elements/PasswordTextBox.hpp>
#include <vulkan/module/overlay/element/elements/cButton.hpp>

class cMainMenu : public cOverlayWindow
{
private:
    iOverlayProvider* ppOverlayProvider;
protected:
    void LoadTextures(cTextureHandler* pTextureHandler)
    {
        pmpTextures["roof"] = pTextureHandler->LoadTextureFromFile("resources/textures/roof.jpg");
        pmpTextures["grey"] = pTextureHandler->LoadTextureFromFile("resources/textures/uvtemplate.bmp");
        pmpTextures["buttonTexture"] = pTextureHandler->LoadTextureFromFile("resources/textures/button.jpg");
        pmpTextures["background"] = pTextureHandler->LoadTextureFromFile("resources/textures/background.jpg");
    }

    void ConstructElements()
    {
        pmpOverlay["oPanel"] = new cStaticElement({(uint)((double)WIDTH * 0.85), (uint)((double)HEIGHT * 0.85)}, pmpTextures["background"]);
        ((cStaticElement*)(pmpOverlay["oPanel"]))->Center();

        pmpOverlay["oUserName"] = new cTextBoxElement({400, 120}, 2, pmpTextures["roof"],
                                                     cOverlayRenderModule::FONT, 13,
                                                     glm::vec3(1, 1, 0));
        pmpOverlay["oUserName"]->SetPosition(glm::vec2(0, 500));

        pmpOverlay["oPassword"] = new cPasswordTextBox({300, 80}, 2, pmpTextures["roof"],
                                                      cOverlayRenderModule::FONT, 13,
                                                      glm::vec3(1, 1, 0));
        pmpOverlay["oPassword"]->SetPosition(glm::vec2(500, 500));

        cButton* oExit = new cButton({40, 40}, 0, pmpTextures["buttonTexture"],
                                       cOverlayRenderModule::FONT, 5,
                                       glm::vec3(0, 0, 0));
        oExit->SetLabel("X");
        oExit->CenterHorizontal();
        oExit->AddX(780);
        oExit->AddY(100);
        oExit->ppaCallbacks.push_back([&] (cButton* poSender) -> void {
            exit(0);
        });
        pmpOverlay["oExit"] = oExit;

        cButton* oSubmit = new cButton({250, 75}, 0, pmpTextures["buttonTexture"],
                                       cOverlayRenderModule::FONT, 12,
                                       glm::vec3(1, 1, 0));
        oSubmit->SetLabel("hallo");
        oSubmit->Center();
        oSubmit->AddY(100);
        oSubmit->ppaCallbacks.push_back([&] (cButton* poSender) -> void {
            HandleOnSubmit();
        });
        pmpOverlay["oSubmit"] = oSubmit;
    }
public:
    cMainMenu(iOverlayProvider* pOverlayProvider) : ppOverlayProvider(pOverlayProvider)
    {
    }

    void HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction)
    {
        cOverlayWindow::HandleMouseButton(uiButton, dXPos, dYPos, iAction);
    }

    void Tick()
    {
    }

    void HandleOnSubmit();

    bool ShouldHandleInput() override
    {
        return true;
    }
};

void cMainMenu::HandleOnSubmit()
{
    std::cout << "submit" << std::endl;
}
