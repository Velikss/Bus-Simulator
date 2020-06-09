#pragma once

#include <pch.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/texture/Texture.hpp>
#include <vulkan/texture/TextureSampler.hpp>

// Class for loading and managing textures
class cTextureHandler
{
private:
    // Device where the textures and sampler are loaded
    cLogicalDevice* ppLogicalDevice;

    // Texture sampler
    cTextureSampler* ppDefaultSampler;
    cTextureSampler* ppSkyboxSampler;

    // Map with all loaded textures
    // The key is the path to the texture
    std::map<string, cTexture*> pmpTextures;

public:
    cTextureHandler(cLogicalDevice* pLogicalDevice);
    ~cTextureHandler(void);

    // Load a texture from a file
    cTexture* LoadFromFile(const string& sFilePath, cTextureSampler* pSampler);
    cTexture* LoadFromFile(const string& sFilePath);

    // Returns the texture sampler
    cTextureSampler* GetDefaultSampler();
    cTextureSampler* GetSkyboxSampler();
};

cTextureHandler::cTextureHandler(cLogicalDevice* pLogicalDevice)
{
    assert(pLogicalDevice != nullptr); // logical device must exist

    // Store the logical device
    ppLogicalDevice = pLogicalDevice;

    ppDefaultSampler = new cTextureSampler(pLogicalDevice,
                                           VK_FILTER_LINEAR,
                                           VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                           true);
    ppSkyboxSampler = new cTextureSampler(pLogicalDevice,
                                          VK_FILTER_NEAREST,
                                          VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                          false);
}

cTextureHandler::~cTextureHandler(void)
{
    // Delete all the textures
    for (auto&[sName, pTexture] : pmpTextures)
    {
        delete pTexture;
    }

    // Delete the texture samplers
    delete ppDefaultSampler;
    delete ppSkyboxSampler;
}

cTexture* cTextureHandler::LoadFromFile(const string& sFilePath, cTextureSampler* pSampler)
{
    cTexture* pTexture;

    // Try and find if this texture has already been loaded
    auto tResult = pmpTextures.find(sFilePath);
    if (tResult == pmpTextures.end())
    {
        // If not, create and load it
        pTexture = new cTexture(ppLogicalDevice, sFilePath, pSampler);
        pTexture->LoadIntoRAM();
        pTexture->LoadIntoGPU();
        pTexture->UnloadFromRAM();
        pmpTextures[sFilePath] = pTexture;
    }
    else
    {
        // If it's already loaded, just grab the loaded texture
        pTexture = tResult->second;
    }

    return pTexture;
}

cTexture* cTextureHandler::LoadFromFile(const string& sFilePath)
{
    return LoadFromFile(sFilePath, GetDefaultSampler());
}

cTextureSampler* cTextureHandler::GetDefaultSampler()
{
    return ppDefaultSampler;
}

cTextureSampler* cTextureHandler::GetSkyboxSampler()
{
    return ppSkyboxSampler;
}
