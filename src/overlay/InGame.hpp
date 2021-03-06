#pragma once
#include <pch.hpp>
#include <vulkan/module/overlay/OverlayWindow.hpp>
#include <vulkan/texture/TextureHandler.hpp>
#include <vulkan/util/GameManager.hpp>
#include <vulkan/module/overlay/element/TextElement.hpp>
#include <vulkan/module/overlay/OverlayRenderModule.hpp>

class cInGame : public cOverlayWindow
{
private:
    iGameManager* ppOverlayProvider;
    uint puiTimeout = 0;
public:
    cInGame(iGameManager* pOverlayProvider) : ppOverlayProvider(pOverlayProvider)
    {}

    void UpdateSpeed(float fSpeed);
    void SetNextStopName(string sNextStop);
    void ShowMissionStatus();
    void HideMissionStatus();
    void ShowDoorOpen();
    void ShowDoorClosed();
protected:
    void LoadTextures(cTextureHandler* pTextureHandler) override
    {
        pmpTextures["overlay"] = pTextureHandler->LoadFromFile("resources/textures/overlay.png");
        pmpTextures["overlay-inv"] = pTextureHandler->LoadFromFile("resources/textures/overlay-inv.png");
        pmpTextures["door-open"] = pTextureHandler->LoadFromFile("resources/textures/door-open.png");
        pmpTextures["door-closed"] = pTextureHandler->LoadFromFile("resources/textures/door-closed.png");
    }

    void ConstructElements() override
    {
        cStaticElement* oMissionOverlay = new cStaticElement({600, 80}, pmpTextures["overlay"]);
        oMissionOverlay->CenterHorizontal();
        oMissionOverlay->Hide();
        pmpOverlay.push_back({"MissionOverlay", oMissionOverlay});

        cTextElement* oNextStop = new cTextElement();
        oNextStop->SetFont(5, cOverlayRenderModule::FONT,
                           glm::vec3(1, 1, 1));
        oNextStop->UpdateText("Next Stop:");
        oNextStop->CenterHorizontal();
        oNextStop->AddY(5);
        oNextStop->Hide();
        pmpOverlay.push_back({"NextStop", oNextStop});

        cTextElement* oNextStopName = new cTextElement();
        oNextStopName->SetFont(10, cOverlayRenderModule::FONT,
                           glm::vec3(1, 1, 1));
        oNextStopName->UpdateText("Mission 1");
        oNextStopName->CenterHorizontal();
        oNextStopName->AddY(25);
        oNextStopName->Hide();
        pmpOverlay.push_back({"NextStopName", oNextStopName});

        // Km/ph

        cStaticElement* oSpeedOverlay = new cStaticElement({600, 140}, pmpTextures["overlay-inv"]);
        oSpeedOverlay->AlignBottom();
        oSpeedOverlay->CenterHorizontal();
        pmpOverlay.push_back({"SpeedOverlay", oSpeedOverlay});

        cTextElement* oKmhValue = new cTextElement();
        oKmhValue->SetFont(25, cOverlayRenderModule::FONT,
                      glm::vec3(1, 1, 1));
        oKmhValue->UpdateText("00");
        oKmhValue->AlignBottom();
        oKmhValue->CenterHorizontal();
        oKmhValue->RemoveY(30);
        pmpOverlay.push_back({"KmhValue", oKmhValue});

        cTextElement* oKmh = new cTextElement();
        oKmh->SetFont(5, cOverlayRenderModule::FONT,
                           glm::vec3(1, 1, 1));
        oKmh->UpdateText("Km/h");
        oKmh->AlignBottom();
        oKmh->CenterHorizontal();
        oKmh->AddX(90);
        oKmh->RemoveY(30);
        pmpOverlay.push_back({"Kmh", oKmh});

        // Door
        cStaticElement* oDoorOpenIcon = new cStaticElement({100, 100}, pmpTextures["door-open"]);
        oDoorOpenIcon->AlignBottom();
        oDoorOpenIcon->CenterHorizontal();
        oDoorOpenIcon->AddX(900);
        oDoorOpenIcon->Hide();
        pmpOverlay.push_back({"DoorOpenIcon", oDoorOpenIcon});

        cStaticElement* oDoorClosedIcon = new cStaticElement({100, 100}, pmpTextures["door-closed"]);
        oDoorClosedIcon->AlignBottom();
        oDoorClosedIcon->CenterHorizontal();
        oDoorClosedIcon->AddX(900);
        pmpOverlay.push_back({"DoorClosedIcon", oDoorClosedIcon});

        cTextElement* oFPS = new cTextElement();
        oFPS->SetFont(6, cOverlayRenderModule::FONT,
                      glm::vec3(0, 1, 0));
        oFPS->SetPosition({10, 10});
        pmpOverlay.push_back({"fps", oFPS});
    }

    void Tick() override
    {
    }

    bool ShouldHandleInput() override
    {
        return false;
    }
};

void cInGame::HideMissionStatus()
{
    GetElement("NextStopName")->Hide();
    GetElement("NextStop")->Hide();
    GetElement("MissionOverlay")->Hide();
}

void cInGame::ShowMissionStatus()
{
    GetElement("NextStopName")->Show();
    GetElement("NextStop")->Show();
    GetElement("MissionOverlay")->Show();
}

void cInGame::ShowDoorClosed()
{
    GetElement("DoorOpenIcon")->Hide();
    GetElement("DoorClosedIcon")->Show();
}

void cInGame::ShowDoorOpen()
{
    GetElement("DoorOpenIcon")->Show();
    GetElement("DoorClosedIcon")->Hide();
}

void cInGame::SetNextStopName(string sNextStop)
{
    dynamic_cast<cTextElement*>(GetElement("NextStopName"))->UpdateText(sNextStop);
    dynamic_cast<cTextElement*>(GetElement("NextStopName"))->CenterHorizontal();
}

void cInGame::UpdateSpeed(float fSpeed)
{
    if(++puiTimeout == 60)
    {
        puiTimeout = 0;
        ((cTextElement*) GetElement("KmhValue"))->UpdateText(to_string_with_precision((fSpeed < 0.0f) ? (fSpeed * -1.0f) : fSpeed, 0));

        ((cTextElement*) GetElement("fps"))->UpdateText(cFormatter() << cProfiler::poInstance.GetFramesPerSecond() << " fps");
    }
}
