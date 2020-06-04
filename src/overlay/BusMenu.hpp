#pragma once

#include <pch.hpp>
#include <overlay/BaseMenu.hpp>
#include <vulkan/module/overlay/element/elements/TextBoxElement.hpp>
#include <vulkan/module/overlay/element/elements/PasswordTextBox.hpp>

class cBusMenu : public cBaseMenu
{
protected:
    iGameManager* ppoOverlayProvider;
    std::vector<cStaticElement*> poBusIcons;
    int piCurrentBus = 0;

    void LoadTextures(cTextureHandler *pTextureHandler);

    void ConstructElements();

public:
    cBusMenu(iGameManager *pOverlayProvider) : cBaseMenu(pOverlayProvider)
    {
        ppoOverlayProvider = pOverlayProvider;
    }

    void HandleOnSubmit(cButton *poSender);

    void HandleSinglePlayer(cButton *poSender);

    bool ShouldHandleInput() override
    {
        return true;
    }

    void PreviousBus(cButton *poSender);
    void NextBus(cButton *poSender);
};

void cBusMenu::LoadTextures(cTextureHandler *pTextureHandler)
{
    cBaseMenu::LoadTextures(pTextureHandler);
    pmpTextures["textbox"] = pTextureHandler->LoadTextureFromFile("resources/textures/textbox.png");
    pmpTextures["bus"] = pTextureHandler->LoadTextureFromFile("resources/textures/penguin.png");

    pmpTextures["bus-yellow-icon"] = pTextureHandler->LoadTextureFromFile("resources/textures/buses/icons/bus-yellow-icon.png");
    pmpTextures["bus-purple-icon"] = pTextureHandler->LoadTextureFromFile("resources/textures/buses/icons/bus-purple-icon.png");
    pmpTextures["bus-red-icon"] = pTextureHandler->LoadTextureFromFile("resources/textures/buses/icons/bus-red-icon.png");
}

void cBusMenu::ConstructElements()
{
    cBaseMenu::ConstructElements();

    poBusIcons = {
            new cStaticElement({400, 400}, pmpTextures["bus-yellow-icon"]),
            new cStaticElement({400, 400}, pmpTextures["bus-purple-icon"]),
            new cStaticElement({400, 400}, pmpTextures["bus-red-icon"])
    };

    for(int i = 0; i < poBusIcons.size(); i++)
    {
        poBusIcons[i]->Center();
        poBusIcons[i]->SetScale(glm::vec2(0, 0));
        pmpOverlay.push_back({"Bus-Icon-" + i, poBusIcons[i]});
    }

    cTextElement *oBusLabel = new cTextElement();
    oBusLabel->SetFont(10, cOverlayRenderModule::FONT,
                       glm::vec3(0, 0, 0));
    oBusLabel->UpdateText("Bus Model:");
    oBusLabel->Center();
    oBusLabel->RemoveY(400);
    pmpOverlay.push_back({"oBusLabel", oBusLabel});

    cButton *oLeftBus = new cButton({250, 75}, 0, pmpTextures["buttonTexture"], cOverlayRenderModule::FONT, 12,
                                    glm::vec3(0, 0, 0));
    oLeftBus->SetLabel("Left");
    oLeftBus->Center();
    oLeftBus->RemoveX(550);
    pmpOverlay.push_back({"oLeftBus", oLeftBus});
    std::function<void(cButton *)> OnPreviousBus = std::bind(&cBusMenu::PreviousBus, this, std::placeholders::_1);
    oLeftBus->ppaCallbacks.push_back(OnPreviousBus);

    cButton *oRightBus = new cButton({250, 75}, 0, pmpTextures["buttonTexture"], cOverlayRenderModule::FONT, 12,
                                     glm::vec3(0, 0, 0));
    oRightBus->SetLabel("Right");
    oRightBus->Center();
    oRightBus->AddX(550);
    pmpOverlay.push_back({"oRightBus", oRightBus});
    std::function<void(cButton *)> OnNextBus = std::bind(&cBusMenu::NextBus, this, std::placeholders::_1);
    oRightBus->ppaCallbacks.push_back(OnNextBus);

    poBusIcons[0]->SetScale(glm::vec2(1, 1));


    cButton *oSubmit = new cButton({400, 75}, 0, pmpTextures["buttonTexture"],
                                   cOverlayRenderModule::FONT, 12,
                                   glm::vec3(0, 0, 0));
    oSubmit->SetLabel("Apply");
    oSubmit->Center();
    oSubmit->AddY(370);
    std::function<void(cButton *)> OnSubmitHandler = std::bind(&cBusMenu::HandleOnSubmit, this, std::placeholders::_1);
    oSubmit->ppaCallbacks.push_back(OnSubmitHandler);

    pmpOverlay.push_back({"oSubmit", oSubmit});

    ((cButton *) GetElement("oExit"))->ppaCallbacks.emplace_back([&](cButton *poSender) -> void
                                                                 {
                                                                     Quit();
                                                                 });
}

void cBusMenu::PreviousBus(cButton *poSender)
{
    poBusIcons[piCurrentBus]->SetScale(glm::vec2(0));

    if((piCurrentBus - 1) >= 0)
        poBusIcons[--piCurrentBus]->SetScale(glm::vec2(1, 1));
    else
    {
        piCurrentBus += poBusIcons.size() - 1;
        poBusIcons[piCurrentBus]->SetScale(glm::vec2(1, 1));
    }
}

void cBusMenu::NextBus(cButton *poSender)
{
    poBusIcons[piCurrentBus]->SetScale(glm::vec2(0));

    if(piCurrentBus + 1 < poBusIcons.size())
        poBusIcons[++piCurrentBus]->SetScale(glm::vec2(1, 1));
    else
    {
        piCurrentBus -= poBusIcons.size() - 1;
        poBusIcons[piCurrentBus]->SetScale(glm::vec2(1, 1));
    }
}

void cBusMenu::HandleOnSubmit(cButton *poSender)
{
    ppoOverlayProvider->GetScenes().at("BusWorld")->;
    std::cout << "submit" << std::endl;
}

