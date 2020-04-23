#pragma once

#include <pch.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/texture/Texture.hpp>

class cTextureHandler
{
private:
    cLogicalDevice* ppLogicalDevice;

    VkSampler poTextureSampler = VK_NULL_HANDLE;

    std::vector<cTexture*> papTextures;

public:
    cTextureHandler(cLogicalDevice* pLogicalDevice);
    ~cTextureHandler(void);

    uint CreateTexture(const char* sFilePath);

    cTexture* GetTexture(uint uiIndex);
    VkSampler GetSampler();

private:
    void CreateTextureSampler(void);
};

cTextureHandler::cTextureHandler(cLogicalDevice* pLogicalDevice)
{
    ppLogicalDevice = pLogicalDevice;

    CreateTextureSampler();
}

cTextureHandler::~cTextureHandler(void)
{
    ppLogicalDevice->DestroySampler(poTextureSampler, nullptr);

    for (cTexture* pTexture : papTextures)
    {
        delete pTexture;
    }
}

uint cTextureHandler::CreateTexture(const char* sFilePath)
{
    // Load the pixel data and image size
    int iTexWidth, iTexHeight, iTexChannels;
    stbi_uc* pcPixels = stbi_load(sFilePath, &iTexWidth, &iTexHeight, &iTexChannels, STBI_rgb_alpha);
    VkDeviceSize ulImageSize = iTexWidth * iTexHeight * 4; // we're using RGBA so 4 byte per pixel

    // check if the texture was loaded correctly
    assert(pcPixels != nullptr);
    assert(iTexWidth > 0);
    assert(iTexHeight > 0);
    assert(ulImageSize > 0);

    // Create a struct with information about the texture
    tTextureInfo tTextureInfo = {};
    tTextureInfo.uiWidth = iTexWidth;
    tTextureInfo.uiHeight = iTexHeight;
    tTextureInfo.uiChannels = iTexChannels;
    tTextureInfo.uiSize = iTexWidth * iTexHeight * 4;

    // Create the texture object
    papTextures.push_back(new cTexture(ppLogicalDevice, tTextureInfo, pcPixels));

    // Return the index of this texture
    return papTextures.size() - 1;
}

void cTextureHandler::CreateTextureSampler(void)
{
    VkSamplerCreateInfo tSamplerInfo = {};
    tSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

    // Filter mode to use to interpolate texels that are magnified or minified
    tSamplerInfo.magFilter = VK_FILTER_LINEAR;
    tSamplerInfo.minFilter = VK_FILTER_LINEAR;

    // The addressing mode to use per axis
    tSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    tSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    tSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    // Configure anisotropic filtering
    tSamplerInfo.anisotropyEnable = VK_TRUE;
    tSamplerInfo.maxAnisotropy = 16; // higher = better quality, max is 16

    // When sampling outside the image, what color to use
    // irrelevant when using REPEAT addressing mode
    tSamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

    // When true,  UV coordinates range from 0 to textureWidth and 0 to textureHeight
    // When false, UV coordinates range from 0 to 1
    tSamplerInfo.unnormalizedCoordinates = VK_FALSE;

    // Allows for comparing texel values to a value and using the result of the comparison
    tSamplerInfo.compareEnable = VK_FALSE;
    tSamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    // Configure mipmapping. Currently disabled
    tSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    tSamplerInfo.mipLodBias = 0.0f;
    tSamplerInfo.minLod = 0.0f;
    tSamplerInfo.maxLod = 0.0f;

    if (!ppLogicalDevice->CreateSampler(&tSamplerInfo, nullptr, &poTextureSampler))
    {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

cTexture* cTextureHandler::GetTexture(uint uiIndex)
{
    assert(uiIndex < papTextures.size());

    return papTextures[uiIndex];
}

VkSampler cTextureHandler::GetSampler()
{
    assert(poTextureSampler != VK_NULL_HANDLE);

    return poTextureSampler;
}
