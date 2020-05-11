#include <pch.hpp>
#include <iostream>
#include <GL/glew.h>
#include <scenes/Scene.hpp>
#include <scenes/MyStreetScene.hpp>
#include <vulkan/vulkan.h>
#include "Engine.hpp"

int main(int argc, char** argv)
{
    Engine engine;

    try {
        engine.Run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

