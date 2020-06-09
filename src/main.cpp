#include "pch.hpp"
#include <iostream>
#include <Engine.hpp>
#include <scenes/BusWorldScene.hpp>
#include <overlay/TestOverlay.hpp>
#include <overlay/MainMenu.hpp>
#include <overlay/LoadingOverlay.hpp>
#include <overlay/BusMenu.hpp>
#include <overlay/SettingsMenu.hpp>
#include <overlay/MissionMenu.hpp>
#include <scenes/MainMenuScene.hpp>

class cApplication : public cEngine
{
    cMultiplayerHandler* ppMultiplayerHandler = nullptr;
    cGameLogicHandler* ppGameLogicHandler = nullptr;
public:
    cApplication() : cEngine("Bus Simulator")
    {
        InitialiseMissions();
    }

protected:
    void LoadMRTShaders(std::vector<string>& shaders) override
    {
        shaders.push_back("resources/shaders/compiled/mrt.vert.spv");
        shaders.push_back("resources/shaders/compiled/mrt.frag.spv");
    }

    void LoadLightingShaders(std::vector<string>& shaders) override
    {
        shaders.push_back("resources/shaders/compiled/lighting.vert.spv");
        shaders.push_back("resources/shaders/compiled/lighting.frag.spv");
    }

    void LoadOverlayShaders(std::vector<string>& shaders) override
    {
        shaders.push_back("resources/shaders/compiled/text.vert.spv");
        shaders.push_back("resources/shaders/compiled/text.frag.spv");
    }

    void SetupScenes(std::map<string, cScene*>& mScenes, string* sInitialScene) override
    {
        mScenes["MainMenu"] = new cMainMenuScene(this);
        mScenes["BusWorld"] = new cBusWorldScene(this, &ppGameLogicHandler);

        *sInitialScene = "MainMenu";
    }

    void LoadOverlayWindows(std::map<string, cOverlayWindow*>& mOverlayWindows) override
    {
        mOverlayWindows["MainMenu"] = new cMainMenu(this, &ppMultiplayerHandler);
        mOverlayWindows["Test"] = new cTestOverlay(this);
        mOverlayWindows["Loading"] = new cLoadingOverlay();
        mOverlayWindows["BusMenu"] = new cBusMenu(this);
        mOverlayWindows["MissionMenu"] = new cMissionMenu(this, &ppGameLogicHandler);
        mOverlayWindows["Settings"] = new cSettingsMenu(this);
        mOverlayWindows["InGame"] = new cInGame(this);
        ppGameLogicHandler->SetGameOverlay(dynamic_cast<cInGame*>(mOverlayWindows["InGame"]));
    }

    void InitialiseMissions()
    {
        ppGameLogicHandler = new cGameLogicHandler();
        ppGameLogicHandler->pmpMissions["Mission1"] = std::make_shared<cMissionHandler>();
        ppGameLogicHandler->pmpMissions["Mission2"] = std::make_shared<cMissionHandler>();
    }
};

int main(int argc, char** argv)
{
    cApplication oGame;

    try
    {
        oGame.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}