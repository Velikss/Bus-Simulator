#pragma once

#include <pch.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/geometry/Geometry.hpp>
#include <vulkan/util/AsyncLoader.hpp>

class cGeometryHandler : public cAsyncLoader<cGeometry>
{
private:
    cLogicalDevice* ppLogicalDevice;

    std::map<string, cGeometry*> pmpGeometries;

public:
    cGeometryHandler(cLogicalDevice* pLogicalDevice);
    ~cGeometryHandler();

    cGeometry* LoadFromFile(const string& sFilePath, const glm::vec2& tUVScale);
    cGeometry* LoadFromFile(const string& sFilePath);

protected:
    void LoadCallback(cGeometry* pObject) override;
};

cGeometryHandler::cGeometryHandler(cLogicalDevice* pLogicalDevice) : cAsyncLoader<cGeometry>(2)
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

void cGeometryHandler::LoadCallback(cGeometry* pObject)
{
    pObject->LoadIntoRAM();
    pObject->CopyIntoGPU();
    pObject->UnloadFromRAM();
}

cGeometry* cGeometryHandler::LoadFromFile(const string& sFilePath, const glm::vec2& tUVScale)
{
    // Try and find if this geometry has already been loaded
    auto tResult = pmpGeometries.find(sFilePath);
    if (tResult == pmpGeometries.end())
    {
        // If not, create and load it
        cGeometry* pGeometry = new cGeometry(ppLogicalDevice, sFilePath, tUVScale);
        pmpGeometries[sFilePath] = pGeometry;
        LoadAsync(pGeometry);
        return pGeometry;
    }
    else
    {
        // If it's already loaded, just grab the loaded geometry
        return tResult->second;
    }
}

cGeometry* cGeometryHandler::LoadFromFile(const string& sFilePath)
{
    return LoadFromFile(sFilePath, glm::vec2(1));
}
