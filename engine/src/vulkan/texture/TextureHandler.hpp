#pragma once

#include <pch.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/texture/Texture.hpp>
#include <vulkan/texture/TextureSampler.hpp>
#include <vulkan/util/AsyncLoader.hpp>

// Class for loading and managing textures
class cTextureHandler : public cAsyncLoader<cTexture>
{
private:
    // Device where the textures and sampler are loaded
    cLogicalDevice* ppLogicalDevice = nullptr;

    // Texture sampler
    cTextureSampler* ppDefaultSampler = nullptr;
    cTextureSampler* ppSkyboxSampler = nullptr;

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
protected:
    void LoadCallback(cTexture* pObject) override;
};

cTextureHandler::cTextureHandler(cLogicalDevice* pLogicalDevice) : cAsyncLoader<cTexture>(4)
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

void cTextureHandler::LoadCallback(cTexture* pObject)
{
    pObject->LoadIntoRAM();
    pObject->CopyIntoGPU();
    pObject->UnloadFromRAM();
}

cTexture* cTextureHandler::LoadFromFile(const string& sFilePath, cTextureSampler* pSampler)
{
    // Try and find if this texture has already been loaded
    auto tResult = pmpTextures.find(sFilePath);
    if (tResult == pmpTextures.end())
    {
        // If not, create and load it
        cTexture* pTexture = new cTexture(ppLogicalDevice, sFilePath, pSampler);
        pmpTextures[sFilePath] = pTexture;
        LoadAsync(pTexture);
        return pTexture;
    }
    else
    {
        // If it's already loaded, just grab the loaded texture
        return tResult->second;
    }
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
