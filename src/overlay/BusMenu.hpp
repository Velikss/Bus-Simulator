#pragma once

#include <pch.hpp>
#include <overlay/BaseMenu.hpp>
#include <vulkan/module/overlay/element/elements/TextBoxElement.hpp>
#include <vulkan/module/overlay/element/elements/PasswordTextBox.hpp>

class cBusMenu : public cBaseMenu
{
protected:
    iGameManager* ppoOverlayProvider;
    std::map<string, cStaticElement*> poBusIcons;
    std::map<string, cStaticElement*>::iterator poBusIterator;

    void LoadTextures(cTextureHandler* pTextureHandler);

    void ConstructElements();

public:
    cBusMenu(iGameManager* pOverlayProvider) : cBaseMenu(pOverlayProvider)
    {
        ppoOverlayProvider = pOverlayProvider;
    }

    void HandleOnSubmit(cButton* poSender);

    void HandleSinglePlayer(cButton* poSender);

    bool ShouldHandleInput() override
    {
        return true;
    }

    void PreviousBus(cButton* poSender);

    void NextBus(cButton* poSender);
};

void cBusMenu::LoadTextures(cTextureHandler* pTextureHandler)
{
    cBaseMenu::LoadTextures(pTextureHandler);
    pmpTextures["textbox"] = pTextureHandler->LoadTextureFromFile("resources/textures/textbox.png");

    // Load all bus icons
    std::string path = "resources/textures/buses/icons";
    for (const auto & entry : std::filesystem::directory_iterator(path))
    {
#if defined(LINUX)
        std::vector<std::string> soPathSplit = split(split(entry.path(), ".")[0], "/");
#elif defined(WINDOWS)
        std::vector<std::string> soPathSplit = split(split(entry.path().string(), ".")[0], "\\");
#endif
        std::string key = soPathSplit[soPathSplit.size() - 1];
        pmpTextures[key] = pTextureHandler->LoadTextureFromFile(entry.path().string().c_str());
        poBusIcons[key] = new cStaticElement({400, 400}, pmpTextures[key]);
    }
}

void cBusMenu::ConstructElements()
{
    cBaseMenu::ConstructElements();

    poBusIterator = poBusIcons.begin();

    for (auto &icon : poBusIcons)
    {
        icon.second->Center();
        icon.second->SetScale(glm::vec2(0, 0));
        pmpOverlay.push_back({icon.first, icon.second});
    }

    cTextElement* oBusLabel = new cTextElement();
    oBusLabel->SetFont(10, cOverlayRenderModule::FONT,
                       glm::vec3(0, 0, 0));
    oBusLabel->UpdateText("Bus Model:");
    oBusLabel->Center();
    oBusLabel->RemoveY(400);
    pmpOverlay.push_back({"oBusLabel", oBusLabel});

    cButton* oLeftBus = new cButton({250, 75}, 0, pmpTextures["buttonTexture"], cOverlayRenderModule::FONT, 12,
                                    glm::vec3(0, 0, 0));
    oLeftBus->SetLabel("Left");
    oLeftBus->Center();
    oLeftBus->RemoveX(550);
    pmpOverlay.push_back({"oLeftBus", oLeftBus});
    std::function<void(cButton*)> OnPreviousBus = std::bind(&cBusMenu::PreviousBus, this, std::placeholders::_1);
    oLeftBus->ppaCallbacks.push_back(OnPreviousBus);

    cButton* oRightBus = new cButton({250, 75}, 0, pmpTextures["buttonTexture"], cOverlayRenderModule::FONT, 12,
                                     glm::vec3(0, 0, 0));
    oRightBus->SetLabel("Right");
    oRightBus->Center();
    oRightBus->AddX(550);
    pmpOverlay.push_back({"oRightBus", oRightBus});
    std::function<void(cButton*)> OnNextBus = std::bind(&cBusMenu::NextBus, this, std::placeholders::_1);
    oRightBus->ppaCallbacks.push_back(OnNextBus);

    poBusIterator->second->SetScale(glm::vec2(1, 1));


    cButton* oSubmit = new cButton({400, 75}, 0, pmpTextures["buttonTexture"],
                                   cOverlayRenderModule::FONT, 12,
                                   glm::vec3(0, 0, 0));
    oSubmit->SetLabel("Apply");
    oSubmit->Center();
    oSubmit->AddY(370);
    std::function<void(cButton*)> OnSubmitHandler = std::bind(&cBusMenu::HandleOnSubmit, this, std::placeholders::_1);
    oSubmit->ppaCallbacks.push_back(OnSubmitHandler);

    pmpOverlay.push_back({"oSubmit", oSubmit});

    ((cButton*) GetElement("oExit"))->ppaCallbacks.emplace_back([&](cButton* poSender) -> void
                                                                {
                                                                    ppoOverlayProvider->DeactivateOverlayWindow();
                                                                });
}

void cBusMenu::PreviousBus(cButton* poSender)
{
    // Hide current skin
    poBusIterator->second->SetScale(glm::vec2(0));

    // If we are not at the begin go to the next, else go back to end
    (poBusIterator != poBusIcons.begin()) ? --poBusIterator : poBusIterator = --poBusIcons.end();

    // Display the icon
    poBusIterator->second->SetScale(glm::vec2(1));
}

void cBusMenu::NextBus(cButton* poSender)
{
    // Hide current skin
    poBusIterator->second->SetScale(glm::vec2(0));

    // If we are not at the end go to next, else go back to begin
    (poBusIterator != (--poBusIcons.end())) ? poBusIterator++ : poBusIterator = poBusIcons.begin();

    // Display the icon
    poBusIterator->second->SetScale(glm::vec2(1));
}

void cBusMenu::HandleOnSubmit(cButton* poSender)
{
    cBusWorldScene* s = static_cast<cBusWorldScene*>(ppoOverlayProvider->GetScenes().at("BusWorld"));
    s->SetBusSkin(split(poBusIterator->first, "-icon")[0]);
}

