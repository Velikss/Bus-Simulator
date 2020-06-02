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
        pmpTextures["textbox"] = pTextureHandler->LoadTextureFromFile("resources/textures/textbox.png");
    }

    void ConstructElements()
    {
        cBaseMenu::ConstructElements();

        cTextBoxElement* oUserName = new cTextBoxElement({400, 120}, 2, pmpTextures["textbox"],
                                                     cOverlayRenderModule::FONT, 13,
                                                      glm::vec3(0,0,0));
        oUserName->SetPosition(glm::vec2(0, 500));
        pmpOverlay.push_back({"oUserName", oUserName});

        cPasswordTextBox* oPassword = new cPasswordTextBox({300, 80}, 2, pmpTextures["textbox"],
                                                      cOverlayRenderModule::FONT, 13,
                                                      glm::vec3(0,0,0));
        oPassword->SetPosition(glm::vec2(500, 500));
        pmpOverlay.push_back({"oPassword", oPassword});

        cButton* oSubmit = new cButton({250, 75}, 0, pmpTextures["buttonTexture"],
                                       cOverlayRenderModule::FONT, 12,
                                       glm::vec3(0,0,0));
        oSubmit->SetLabel("hallo");
        oSubmit->Center();
        oSubmit->AddY(100);
        pmpOverlay.push_back({"oSubmit", oSubmit});

        ((cButton*)GetElement("oExit"))->ppaCallbacks.push_back([&] (cButton* poSender) -> void {
            exit(0);
        });
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
