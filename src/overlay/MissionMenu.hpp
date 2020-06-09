#pragma once

#include <pch.hpp>
#include <overlay/BaseMenu.hpp>
#include <vulkan/module/overlay/element/elements/TextBoxElement.hpp>
#include <vulkan/module/overlay/element/elements/PasswordTextBox.hpp>
#include <scenes/BusWorldScene.hpp>

class cMissionMenu : public cBaseMenu
{
protected:
    iGameManager* ppoOverlayProvider;
    std::map<string, cStaticElement*> poMissionIcons;
    std::map<string, cStaticElement*>::iterator poMissionIconIterator;
    cGameLogicHandler** pGameLogicHandler;

    void LoadTextures(cTextureHandler* pTextureHandler) override;

    void ConstructElements() override;

public:
    cMissionMenu(iGameManager* pOverlayProvider, cGameLogicHandler** oGameLogicHandler) : cBaseMenu(pOverlayProvider)
    {
        ppoOverlayProvider = pOverlayProvider;
        pGameLogicHandler = oGameLogicHandler;
    }

    void HandleOnSubmit(cButton* poSender);

    bool ShouldHandleInput() override
    {
        return true;
    }

    void PreviousMission(cButton* poSender);

    void NextMission(cButton* poSender);
};

void cMissionMenu::LoadTextures(cTextureHandler* pTextureHandler)
{
    cBaseMenu::LoadTextures(pTextureHandler);

    // Load all Mission icons
    std::string path = "resources/textures/missions";
    for (const auto & entry : std::filesystem::directory_iterator(path))
    {
#if defined(LINUX)
        std::vector<std::string> soPathSplit = split(split(entry.path(), ".")[0], "/");
#elif defined(WINDOWS)
        std::vector<std::string> soPathSplit = split(split(entry.path().string(), ".")[0], "\\");
#endif
        std::string key = soPathSplit[soPathSplit.size() - 1];
        pmpTextures[key] = pTextureHandler->LoadFromFile(entry.path().string().c_str());
        poMissionIcons[key] = new cStaticElement({574, 367}, pmpTextures[key]);
    }
}

void cMissionMenu::ConstructElements()
{
    cBaseMenu::ConstructElements();

    poMissionIconIterator = poMissionIcons.begin();

    for (auto& [sKey, poMissionHandler] : (*pGameLogicHandler)->pmpMissions)
    {
        if(poMissionIcons.find(sKey) != poMissionIcons.end())
        {
            poMissionIcons[sKey]->Center();
            poMissionIcons[sKey]->SetScale(glm::vec2(0, 0));
            pmpOverlay.push_back({sKey, poMissionIcons[sKey]});
        }
        else
            throw std::runtime_error(sKey + " MISSION ICON DOES NOT EXIST");
    }

    cTextElement* oBusLabel = new cTextElement();
    oBusLabel->SetFont(15, cOverlayRenderModule::FONT,
                       glm::vec3(0, 0, 0));
    oBusLabel->UpdateText("Mission:");
    oBusLabel->Center();
    oBusLabel->RemoveY(400);
    pmpOverlay.push_back({"oMissionLabel", oBusLabel});


    cButton* oLeftMission = new cButton({50, 75}, 0, pmpTextures["buttonTexture"], cOverlayRenderModule::FONT, 12,
                                        glm::vec3(0, 0, 0));
    oLeftMission->SetLabel("<");
    oLeftMission->Center();
    oLeftMission->RemoveX(550);
    pmpOverlay.push_back({"oLeftMission", oLeftMission});
    std::function<void(cButton*)> OnPreviousBus = std::bind(&cMissionMenu::PreviousMission, this, std::placeholders::_1);
    oLeftMission->ppaCallbacks.push_back(OnPreviousBus);

    cButton* oRightMission = new cButton({50, 75}, 0, pmpTextures["buttonTexture"], cOverlayRenderModule::FONT, 12,
                                         glm::vec3(0, 0, 0));
    oRightMission->SetLabel(">");
    oRightMission->Center();
    oRightMission->AddX(550);
    pmpOverlay.push_back({"oRightMission", oRightMission});
    std::function<void(cButton*)> OnNextBus = std::bind(&cMissionMenu::NextMission, this, std::placeholders::_1);
    oRightMission->ppaCallbacks.push_back(OnNextBus);

    poMissionIconIterator->second->SetScale(glm::vec2(1, 1));


    cButton* oSubmit = new cButton({400, 75}, 0, pmpTextures["buttonTexture"],
                                   cOverlayRenderModule::FONT, 12,
                                   glm::vec3(0, 0, 0));
    oSubmit->SetLabel("Start mission");
    oSubmit->Center();
    oSubmit->AddY(370);
    std::function<void(cButton*)> OnSubmitHandler = std::bind(&cMissionMenu::HandleOnSubmit, this, std::placeholders::_1);
    oSubmit->ppaCallbacks.push_back(OnSubmitHandler);

    pmpOverlay.push_back({"oSubmit", oSubmit});
}

void cMissionMenu::PreviousMission(cButton* poSender)
{
    // Hide current mission icon
    poMissionIconIterator->second->SetScale(glm::vec2(0));

    // If we are not at the begin go to the next, else go back to end
    (poMissionIconIterator != poMissionIcons.begin()) ? --poMissionIconIterator : poMissionIconIterator = --poMissionIcons.end();

    // Display the mission icon
    poMissionIconIterator->second->SetScale(glm::vec2(1));
}

void cMissionMenu::NextMission(cButton* poSender)
{
    // Hide current mission icon
    poMissionIconIterator->second->SetScale(glm::vec2(0));

    // If we are not at the end go to next, else go back to begin
    (poMissionIconIterator != (--poMissionIcons.end())) ? poMissionIconIterator++ : poMissionIconIterator = poMissionIcons.begin();

    // Display the mission icon
    poMissionIconIterator->second->SetScale(glm::vec2(1));
}

void cMissionMenu::HandleOnSubmit(cButton* poSender)
{
    (*pGameLogicHandler)->LoadMission(poMissionIconIterator->first,
                                      static_cast<cBusWorldScene *>(ppoOverlayProvider->GetScenes().at("BusWorld")));
    ppoOverlayProvider->DeactivateOverlayWindow();

}