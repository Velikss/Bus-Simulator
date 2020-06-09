#pragma once

#include <pch.hpp>
#include <overlay/BaseMenu.hpp>
#include <vulkan/module/overlay/element/elements/TextBoxElement.hpp>
#include <vulkan/module/overlay/element/elements/PasswordTextBox.hpp>
#include <Json.hpp>

class cMainMenu : public cBaseMenu
{
    cMultiplayerHandler** pppoMultiplayerHandler = nullptr;
    cNetworkConnection::tNetworkInitializationSettings ptConnectNetworkSettings = {};
    nlohmann::json poJson;
protected:
    void LoadTextures(cTextureHandler* pTextureHandler) override
    {
        cBaseMenu::LoadTextures(pTextureHandler);
        pmpTextures["textbox"] = pTextureHandler->LoadFromFile("resources/textures/textbox.png");
    }

    void ConstructElements() override
    {
        cBaseMenu::ConstructElements();
        GetElement("Background")->SetScale(glm::vec2(0));

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

        cButton* oRegister = new cButton({400, 75}, 0, pmpTextures["buttonTexture"],
                                       cOverlayRenderModule::FONT, 10,
                                       glm::vec3(0,0,0));
        oRegister->SetLabel("Register");
        oRegister->Center();
        oRegister->AddY(260);
        std::function<void(cButton*)> OnRegisterHandler = std::bind(&cMainMenu::HandleRegister, this, std::placeholders::_1);
        oRegister->ppaCallbacks.push_back(OnRegisterHandler);
        pmpOverlay.push_back({"oRegister", oRegister});

        cButton* oSinglePlayer = new cButton({400, 75}, 0, pmpTextures["buttonTexture"],
                                       cOverlayRenderModule::FONT, 10,
                                       glm::vec3(0,0,0));
        oSinglePlayer->SetLabel("Singleplayer");
        oSinglePlayer->Center();
        oSinglePlayer->AddY(350);
        std::function<void(cButton*)> OnSinglePlayerHandler = std::bind(&cMainMenu::HandleSinglePlayer, this, std::placeholders::_1);
        oSinglePlayer->ppaCallbacks.push_back(OnSinglePlayerHandler);

        pmpOverlay.push_back({"oSinglePlayer", oSinglePlayer});

        ((cButton*)GetElement("oExit"))->ppaCallbacks.emplace_back([&] (cButton* poSender) -> void {
            Quit();
        });
    }
public:
    cMainMenu(iGameManager* pOverlayProvider, cMultiplayerHandler** ppMultiplayerHandler) : cBaseMenu(pOverlayProvider)
    {
        pppoMultiplayerHandler = ppMultiplayerHandler;
    }

    void HandleOnSubmit(cButton* poSender);
    void HandleSinglePlayer(cButton* poSender);
    void HandleRegister(cButton* poSender);

    bool ShouldHandleInput() override
    {
        return true;
    }
};

void cMainMenu::HandleOnSubmit(cButton* poSender)
{
    cTextBoxElement* poUserName = (cTextBoxElement*)GetElement("oUserName");
    cPasswordTextBox* poPassword = (cPasswordTextBox*)GetElement("oPassword");

    poJson = {};
    std::ifstream oConfigStream("./config.json");
    if (!oConfigStream.is_open()) throw std::runtime_error("could not find config.");
    oConfigStream >> poJson;
    oConfigStream.close();
    if (!poJson.contains("Multiplayer")) throw std::runtime_error("could not find multiplayer settings");
    if (!poJson["Multiplayer"].contains("IP")) throw std::runtime_error("could not find server-ip setting");
    if (!poJson["Multiplayer"].contains("PORT")) throw std::runtime_error("could not find server-port setting");

    string sPort = poJson["Multiplayer"]["PORT"];
    ushort usPort = (ushort)strtoul(sPort.c_str(), NULL, 0);

    ptConnectNetworkSettings.sAddress = cNetworkAbstractions::DNSLookup(poJson["Multiplayer"]["IP"]);
    ptConnectNetworkSettings.usPort = usPort;
    ptConnectNetworkSettings.eMode = cNetworkConnection::cMode::eNonBlocking;
    if (poJson["Multiplayer"].contains("USESSL"))
        ptConnectNetworkSettings.bUseSSL = poJson["Multiplayer"]["USESSL"];

    poJson.clear();
    std::cout << "Connecting to: " << ptConnectNetworkSettings.sAddress << ":" << ptConnectNetworkSettings.usPort << ", SSL: " << (ptConnectNetworkSettings.bUseSSL ? "true" : "false") << std::endl;
    (*pppoMultiplayerHandler) = new cMultiplayerHandler(&ptConnectNetworkSettings);
    if((*pppoMultiplayerHandler)->Connect())
    {
        if ((*pppoMultiplayerHandler)->Login(poUserName->GetValue(), poPassword->GetValue()))
        {
            std::cout << "Successfully logged in." << std::endl;
            ppGameManager->ActivateOverlayWindow("Loading");
            ppGameManager->SwitchScene("BusWorld");
            (*pppoMultiplayerHandler)->StartMultiplayerSession();
            ((cBusWorldScene*)ppGameManager->GetScenes()["BusWorld"])->AssignMultiplayerHandler((*pppoMultiplayerHandler) );
            std::cout << "Entering multiplayer mode" << std::endl;
            return;
        }
    }
    std::cout << "Failed to log in." << std::endl;
    (*pppoMultiplayerHandler)->Disconnect();
    delete (*pppoMultiplayerHandler);
}

void cMainMenu::HandleSinglePlayer(cButton* poSender)
{
    ppGameManager->ActivateOverlayWindow("Loading");
    ppGameManager->SwitchScene("BusWorld");
    std::cout << "entering singleplayer mode" << std::endl;
}

void cMainMenu::HandleRegister(cButton* poSender)
{
    cTextBoxElement* poUserName = (cTextBoxElement*)GetElement("oUserName");
    cPasswordTextBox* poPassword = (cPasswordTextBox*)GetElement("oPassword");
    poJson = {};
    std::ifstream oConfigStream("./config.json");
    if (!oConfigStream.is_open()) throw std::runtime_error("could not find config.");
    oConfigStream >> poJson;
    oConfigStream.close();
    if (!poJson.contains("Multiplayer")) throw std::runtime_error("could not find multiplayer settings");
    if (!poJson["Multiplayer"].contains("IP")) throw std::runtime_error("could not find server-ip setting");
    if (!poJson["Multiplayer"].contains("PORT")) throw std::runtime_error("could not find server-port setting");

    string sPort = poJson["Multiplayer"]["PORT"];
    ushort usPort = (ushort)strtoul(sPort.c_str(), NULL, 0);

    ptConnectNetworkSettings.sAddress = cNetworkAbstractions::DNSLookup(poJson["Multiplayer"]["IP"]);
    ptConnectNetworkSettings.usPort = usPort;
    ptConnectNetworkSettings.eMode = cNetworkConnection::cMode::eNonBlocking;
    poJson.clear();
    std::cout << "Connecting to: " << ptConnectNetworkSettings.sAddress << ":" << ptConnectNetworkSettings.usPort << std::endl;
    (*pppoMultiplayerHandler) = new cMultiplayerHandler(&ptConnectNetworkSettings);
    if((*pppoMultiplayerHandler)->Connect())
    {
        if ((*pppoMultiplayerHandler)->RegisterUser(poUserName->GetValue(), poPassword->GetValue()))
        {
            std::cout << "Successfully registered user." << std::endl;
            return;
        }
    }
    std::cout << "Failed to register user." << std::endl;
    (*pppoMultiplayerHandler)->Disconnect();
    delete (*pppoMultiplayerHandler);
}
