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

        cTextElement* oUserNameLabel = new cTextElement();
        oUserNameLabel->SetFont(8, cOverlayRenderModule::FONT,
                                glm::vec3(0,0,0));
        oUserNameLabel->UpdateText("Username:");
        oUserNameLabel->Center();
        oUserNameLabel->RemoveY(150);
        pmpOverlay.push_back({"oUserNameLabel", oUserNameLabel});

        cTextBoxElement* oUserName = new cTextBoxElement({400, 80}, 2, pmpTextures["textbox"],
                                                     cOverlayRenderModule::FONT, 13,
                                                      glm::vec3(0,0,0));
        oUserName->Center();
        oUserName->RemoveY(80);
        pmpOverlay.push_back({"oUserName", oUserName});

        cPasswordTextBox* oPassword = new cPasswordTextBox({400, 80}, 2, pmpTextures["textbox"],
                                                      cOverlayRenderModule::FONT, 13,
                                                      glm::vec3(0,0,0));
        oPassword->Center();
        oPassword->AddY(80);
        pmpOverlay.push_back({"oPassword", oPassword});

        cButton* oSubmit = new cButton({400, 75}, 0, pmpTextures["buttonTexture"],
                                       cOverlayRenderModule::FONT, 12,
                                       glm::vec3(0,0,0));
        oSubmit->SetLabel("Login");
        oSubmit->Center();
        oSubmit->AddY(170);
        std::function<void(cButton*)> OnSubmitHandler = std::bind(&cMainMenu::HandleOnSubmit, this, std::placeholders::_1);
        oSubmit->ppaCallbacks.push_back(OnSubmitHandler);

        pmpOverlay.push_back({"oSubmit", oSubmit});

        ((cButton*)GetElement("oExit"))->ppaCallbacks.emplace_back([&] (cButton* poSender) -> void {
            Quit();
        });
    }
public:
    cMainMenu(iOverlayProvider* pOverlayProvider) : cBaseMenu(pOverlayProvider)
    {
    }

    void HandleOnSubmit(cButton* poSender);

    bool ShouldHandleInput() override
    {
        return true;
    }
};

void cMainMenu::HandleOnSubmit(cButton* poSender)
{
    std::cout << "submit" << std::endl;
}
