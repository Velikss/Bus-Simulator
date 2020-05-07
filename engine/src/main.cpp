#include <pch.hpp>
#include <iostream>
#include <Engine.hpp>

int main(int argc, char** argv)
{
    Engine oEngine;

    try
    {
        oEngine.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
