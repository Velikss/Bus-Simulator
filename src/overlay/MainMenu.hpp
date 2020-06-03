#pragma once

#include <pch.hpp>
#include <overlay/BaseMenu.hpp>
#include <vulkan/module/overlay/element/elements/TextBoxElement.hpp>
#include <vulkan/module/overlay/element/elements/PasswordTextBox.hpp>

class cMainMenu : public cBaseMenu
{
protected:
    void LoadTextures(cTextureHandler* pTextureHandler) override
    {
        cBaseMenu::LoadTextures(pTextureHandler);
        pmpTextures["textbox"] = pTextureHandler->LoadTextureFromFile("resources/textures/textbox.png");
    }

    void ConstructElements() override
    {
        cBaseMenu::ConstructElements();

        cTextElement* oUserNameLabel = new cTextElement();
        oUserNameLabel->SetFont(8, cOverlayRenderModule::FONT,
                                glm::vec3(0,0,0));
        oUserNameLabel->UpdateText("Username:");
        oUserNameLabel->Center();
        oUserNameLabel->RemoveY(155);
        oUserNameLabel->RemoveX(100);
        pmpOverlay.push_back({"oUserNameLabel", oUserNameLabel});

        cTextBoxElement* oUserName = new cTextBoxElement({400, 80}, 2, pmpTextures["textbox"],
                                                     cOverlayRenderModule::FONT, 13,
                                                      glm::vec3(0,0,0));
        oUserName->Center();
        oUserName->RemoveY(80);
        pmpOverlay.push_back({"oUserName", oUserName});


        cTextElement* oPasswordLabel = new cTextElement();
        oPasswordLabel->SetFont(8, cOverlayRenderModule::FONT,
                                glm::vec3(0,0,0));
        oPasswordLabel->UpdateText("Password:");
        oPasswordLabel->Center();
        oPasswordLabel->AddY(10);
        oPasswordLabel->RemoveX(100);
        pmpOverlay.push_back({"oPasswordLabel", oPasswordLabel});

        cPasswordTextBox* oPassword = new cPasswordTextBox({400, 80}, 2, pmpTextures["textbox"],
                                                      cOverlayRenderModule::FONT, 13,
                                                      glm::vec3(0,0,0));
        oPassword->Center();
        oPassword->AddY(80);
        pmpOverlay.push_back({"oPassword", oPassword});

        cButton* oSubmit = new cButton({400, 75}, 0, pmpTextures["buttonTexture"],
                                       cOverlayRenderModule::FONT, 10,
                                       glm::vec3(0,0,0));
        oSubmit->SetLabel("Login");
        oSubmit->Center();
        oSubmit->AddY(170);
        std::function<void(cButton*)> OnSubmitHandler = std::bind(&cMainMenu::HandleOnSubmit, this, std::placeholders::_1);
        oSubmit->ppaCallbacks.push_back(OnSubmitHandler);

        pmpOverlay.push_back({"oSubmit", oSubmit});

        cButton* oSinglePlayer = new cButton({400, 75}, 0, pmpTextures["buttonTexture"],
                                       cOverlayRenderModule::FONT, 10,
                                       glm::vec3(0,0,0));
        oSinglePlayer->SetLabel("Singleplayer");
        oSinglePlayer->Center();
        oSinglePlayer->AddY(270);
        std::function<void(cButton*)> OnSinglePlayerHandler = std::bind(&cMainMenu::HandleSinglePlayer, this, std::placeholders::_1);
        oSinglePlayer->ppaCallbacks.push_back(OnSinglePlayerHandler);

        pmpOverlay.push_back({"oSinglePlayer", oSinglePlayer});

        ((cButton*)GetElement("oExit"))->ppaCallbacks.emplace_back([&] (cButton* poSender) -> void {
            Quit();
        });
    }
public:
    cMainMenu(iGameManager* pOverlayProvider) : cBaseMenu(pOverlayProvider)
    {
    }

    void HandleOnSubmit(cButton* poSender);
    void HandleSinglePlayer(cButton* poSender);

    bool ShouldHandleInput() override
    {
        return true;
    }
};

void cMainMenu::HandleOnSubmit(cButton* poSender)
{
    cTextBoxElement* poUserName = (cTextBoxElement*)GetElement("oUserName");
    cPasswordTextBox* poPassword = (cPasswordTextBox*)GetElement("oPassword");
    std::cout << "logging in with credentials: " << poUserName->GetValue() << ":" << poPassword->GetValue();
}

void cMainMenu::HandleSinglePlayer(cButton* poSender)
{
    ppOverlayProvider->ActivateOverlayWindow("Loading");
    ppOverlayProvider->SwitchScene("BusWorld");
    std::cout << "entering singleplayer mode" << std::endl;
}
