#pragma once

#include <pch.hpp>
#include <overlay/BaseMenu.hpp>
#include <vulkan/module/overlay/element/elements/TextBoxElement.hpp>
#include <vulkan/module/overlay/element/elements/PasswordTextBox.hpp>

class cBusMenu : public cBaseMenu
{
protected:
    void LoadTextures(cTextureHandler *pTextureHandler);

    void ConstructElements();

    std::vector<cStaticElement*> poBusIcons;

public:
    cBusMenu(iGameManager *pOverlayProvider) : cBaseMenu(pOverlayProvider)
    {
    }

    void HandleOnSubmit(cButton *poSender);

    void HandleSinglePlayer(cButton *poSender);

    bool ShouldHandleInput() override
    {
        return true;
    }
};

void cBusMenu::HandleOnSubmit(cButton *poSender)
{
    std::cout << "submit" << std::endl;
}


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

    for(auto& element : poBusIcons)
    {
        element->Center();
        element->SetScale(glm::vec2(0, 0));
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

    cButton *oRightBus = new cButton({250, 75}, 0, pmpTextures["buttonTexture"], cOverlayRenderModule::FONT, 12,
                                     glm::vec3(0, 0, 0));
    oRightBus->SetLabel("Right");
    oRightBus->Center();
    oRightBus->AddX(550);
    pmpOverlay.push_back({"oRightBus", oRightBus});

    cStaticElement *oBusImage = new cStaticElement({400, 400}, pmpTextures["bus"]);
    oBusImage->Center();
    pmpOverlay.push_back({"oBusImage", oBusImage});


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

// void cBusMenu::SelectBus()
// {
//
// }
