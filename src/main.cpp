#include "pch.hpp"
#include <iostream>
#include <Engine.hpp>
#include <scenes/BusWorldScene.hpp>

class cApplication : public Engine
{
public:
    cApplication()
    {
        ppScene = new cBusWorldScene();
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