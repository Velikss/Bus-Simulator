#pragma once

#include <pch.hpp>
#include <overlay/BaseMenu.hpp>
#include <vulkan/module/overlay/element/elements/TextBoxElement.hpp>
#include <vulkan/module/overlay/element/elements/PasswordTextBox.hpp>

class cBusMenu : public cBaseMenu
{
protected:
    iGameManager *ppoOverlayProvider;
    std::map<string, cStaticElement *> poBusIcons;
    std::map<string, cStaticElement *>::iterator poBusIterator;

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

    pmpTextures["bus-yellow-icon"] = pTextureHandler->LoadTextureFromFile(
            "resources/textures/buses/icons/bus-yellow-icon.png");
    pmpTextures["bus-purple-icon"] = pTextureHandler->LoadTextureFromFile(
            "resources/textures/buses/icons/bus-purple-icon.png");
    pmpTextures["bus-red-icon"] = pTextureHandler->LoadTextureFromFile(
            "resources/textures/buses/icons/bus-red-icon.png");
}

void cBusMenu::ConstructElements()
{
    cBaseMenu::ConstructElements();

    poBusIcons = {
            {"yellow", new cStaticElement({400, 400}, pmpTextures["bus-yellow-icon"])},
            {"purple", new cStaticElement({400, 400}, pmpTextures["bus-purple-icon"])},
            {"red",    new cStaticElement({400, 400}, pmpTextures["bus-red-icon"])}
    };

    poBusIterator = poBusIcons.find("yellow");

    int i = 0;
    for (auto &icon : poBusIcons)
    {
        icon.second->Center();
        icon.second->SetScale(glm::vec2(0, 0));
        pmpOverlay.push_back({"Bus-Icon-" + i, icon.second});
        i++;
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

    poBusIterator->second->SetScale(glm::vec2(1, 1));


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
    // Hide current skin
    poBusIterator->second->SetScale(glm::vec2(0, 0));

    // If we are not at the begin go to the next, else go back to end
    (poBusIterator != poBusIcons.begin()) ? --poBusIterator : poBusIterator = poBusIcons.end();

    // Display the icon
    poBusIterator->second->SetScale(glm::vec2(1, 1));
}

void cBusMenu::NextBus(cButton *poSender)
{
    // Hide current skin
    poBusIterator->second->SetScale(glm::vec2(0, 0));

    // If we are not at the end go to next, else go back to begin
    if (poBusIterator != poBusIcons.end())
    {
        poBusIterator++;
    } else
    {
        poBusIterator = poBusIcons.begin();
    }

    // Display the icon
    poBusIterator->second->SetScale(glm::vec2(1, 1));
}

void cBusMenu::HandleOnSubmit(cButton *poSender)
{
    std::cout << "submit" << std::endl;
}

