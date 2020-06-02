#pragma once

#include <pch.hpp>
#include <overlay/BaseMenu.hpp>
#include <vulkan/module/overlay/element/elements/TextBoxElement.hpp>
#include <vulkan/module/overlay/element/elements/PasswordTextBox.hpp>

class cMainMenu : public cBaseMenu
{
private:
    iOverlayProvider* ppOverlayProvider;
protected:
    void LoadTextures(cTextureHandler* pTextureHandler)
    {
        cBaseMenu::LoadTextures(pTextureHandler);
        pmpTextures["roof"] = pTextureHandler->LoadTextureFromFile("resources/textures/roof.jpg");
        pmpTextures["grey"] = pTextureHandler->LoadTextureFromFile("resources/textures/uvtemplate.bmp");
    }

    void ConstructElements()
    {
        cBaseMenu::ConstructElements();

        pmpOverlay["oUserName"] = new cTextBoxElement({400, 120}, 2, pmpTextures["roof"],
                                                     cOverlayRenderModule::FONT, 13,
                                                     glm::vec3(1, 1, 0));
        pmpOverlay["oUserName"]->SetPosition(glm::vec2(0, 500));

        pmpOverlay["oPassword"] = new cPasswordTextBox({300, 80}, 2, pmpTextures["roof"],
                                                      cOverlayRenderModule::FONT, 13,
                                                      glm::vec3(1, 1, 0));
        pmpOverlay["oPassword"]->SetPosition(glm::vec2(500, 500));

        cButton* oSubmit = new cButton({250, 75}, 0, pmpTextures["buttonTexture"],
                                       cOverlayRenderModule::FONT, 12,
                                       glm::vec3(1, 1, 0));
        oSubmit->SetLabel("hallo");
        oSubmit->Center();
        oSubmit->AddY(100);
        ((cButton*)pmpOverlay["oExit"])->ppaCallbacks.push_back([&] (cButton* poSender) -> void {
            exit(0);
        });
        pmpOverlay["oSubmit"] = oSubmit;
    }
public:
    cMainMenu(iOverlayProvider* pOverlayProvider) : cBaseMenu(pOverlayProvider)
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
