#pragma once

#include <pch.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/geometry/Geometry.hpp>

class cGeometryHandler
{
private:
    cLogicalDevice* ppLogicalDevice;

    std::map<string, cGeometry*> pmpGeometries;

public:
    cGeometryHandler(cLogicalDevice* pLogicalDevice);
    ~cGeometryHandler();

    cGeometry* LoadFromFile(const string& sFilePath, const glm::vec2& tUVScale);
    cGeometry* LoadFromFile(const string& sFilePath);
};

cGeometryHandler::cGeometryHandler(cLogicalDevice* pLogicalDevice)
{
    assert(pLogicalDevice != nullptr);
    ppLogicalDevice = pLogicalDevice;
}

cGeometryHandler::~cGeometryHandler()
{
    for (auto&[sName, pGeometry] : pmpGeometries)
    {
        delete pGeometry;
    }
}

cGeometry* cGeometryHandler::LoadFromFile(const string& sFilePath, const glm::vec2& tUVScale)
{
    cGeometry* pGeometry;

    // Try and find if this geometry has already been loaded
    auto tResult = pmpGeometries.find(sFilePath);
    if (tResult == pmpGeometries.end())
    {
        // If not, create and load it
        pGeometry = new cGeometry(ppLogicalDevice, sFilePath, tUVScale);
        pGeometry->LoadIntoRAM();
        pGeometry->LoadIntoGPU();
        pGeometry->UnloadFromRAM();
        pmpGeometries[sFilePath] = pGeometry;
    }
    else
    {
        // If it's already loaded, just grab the loaded geometry
        pGeometry = tResult->second;
    }

    return pGeometry;
}

cGeometry* cGeometryHandler::LoadFromFile(const string& sFilePath)
{
    return LoadFromFile(sFilePath, glm::vec2(1));
}
