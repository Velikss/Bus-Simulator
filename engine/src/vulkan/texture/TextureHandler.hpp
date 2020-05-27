#pragma once

#include <pch.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/texture/Texture.hpp>
#include <util/Formatter.hpp>
#include "TextureSampler.hpp"

// Class for loading and managing textures
class cTextureHandler
{
private:
    // Device where the textures and sampler are loaded
    cLogicalDevice* ppLogicalDevice;

    // Texture sampler
    cTextureSampler* ppDefaultSampler;
    cTextureSampler* ppSkyboxSampler;

public:
    cTextureHandler(cLogicalDevice* pLogicalDevice);
    ~cTextureHandler(void);

    // Load a texture from a file
    cTexture* LoadTextureFromFile(const char* sFilePath, cTextureSampler* pSampler);
    cTexture* LoadTextureFromFile(const char* sFilePath);

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
    delete ppDefaultSampler;
    delete ppSkyboxSampler;
}

cTexture* cTextureHandler::LoadTextureFromFile(const char* sFilePath, cTextureSampler* pSampler)
{
    // Load the pixel data and image size
    int iTexWidth, iTexHeight;
    stbi_uc* pcPixels = stbi_load(sFilePath, &iTexWidth, &iTexHeight, nullptr, STBI_rgb_alpha);

    // Make sure the texture was loaded correctly
    if (pcPixels == nullptr)
    {
        throw std::runtime_error(cFormatter() << "unable to load texture '" << sFilePath << "'");
    }

    // Check if the texture is valid
    assert(iTexWidth > 0);
    assert(iTexHeight > 0);

    // Create a struct with information about the texture
    tTextureInfo tTextureInfo = {};
    tTextureInfo.uiWidth = iTexWidth;
    tTextureInfo.uiHeight = iTexHeight;
    tTextureInfo.uiSize = iTexWidth * iTexHeight * 4; // we're using RGBA so 4 byte per pixel
    tTextureInfo.uiMipLevels = (uint)std::floor(std::log2(std::max(iTexWidth, iTexHeight))) + 1;

    // Create the texture object and return it
    return new cTexture(ppLogicalDevice, tTextureInfo, pcPixels, sFilePath, pSampler);
}

cTexture* cTextureHandler::LoadTextureFromFile(const char* sFilePath)
{
    return LoadTextureFromFile(sFilePath, GetDefaultSampler());
}

cTextureSampler* cTextureHandler::GetDefaultSampler()
{
    return ppDefaultSampler;
}

cTextureSampler* cTextureHandler::GetSkyboxSampler()
{
    return ppSkyboxSampler;
}
