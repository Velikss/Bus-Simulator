#include "pch.hpp"
#include <iostream>
#include <Engine.hpp>
#include <scenes/BusWorldScene.hpp>
#include <overlay/TestOverlay.hpp>
#include <overlay/MainMenu.hpp>

class cApplication : public cEngine
{
public:
    cApplication() : cEngine("Bus Simulator")
    {
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

    void LoadScene(cScene** pScene) override
    {
        *pScene = new cBusWorldScene(this);
    }

    void LoadOverlayWindows(std::map<string, cOverlayWindow*>& mOverlayWindows) override
    {
        mOverlayWindows["MainMenu"] = new cMainMenu(this);
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