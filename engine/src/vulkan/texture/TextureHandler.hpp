#pragma once

#include <pch.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/texture/Texture.hpp>

// Class for loading and managing textures
class cTextureHandler
{
private:
    // Device where the textures and sampler are loaded
    cLogicalDevice* ppLogicalDevice;

    // Texture sampler object
    VkSampler poTextureSampler = VK_NULL_HANDLE;

public:
    cTextureHandler(cLogicalDevice* pLogicalDevice);
    ~cTextureHandler(void);

    // Load a texture from a file
    cTexture* LoadTextureFromFile(const char* sFilePath);

    // Returns the texture sampler
    VkSampler GetSampler();

private:
    // Creates the texture sampler
    void CreateTextureSampler(void);
};

cTextureHandler::cTextureHandler(cLogicalDevice* pLogicalDevice)
{
    assert(pLogicalDevice != nullptr); // logical device must exist

    // Store the logical device
    ppLogicalDevice = pLogicalDevice;

    // Create the texture sampler
    CreateTextureSampler();
}

cTextureHandler::~cTextureHandler(void)
{
    // Destroy the texture sampler
    ppLogicalDevice->DestroySampler(poTextureSampler, nullptr);
}

cTexture* cTextureHandler::LoadTextureFromFile(const char* sFilePath)
{
    // Load the pixel data and image size
    int iTexWidth, iTexHeight;
    stbi_uc* pcPixels = stbi_load(sFilePath, &iTexWidth, &iTexHeight, nullptr, STBI_rgb_alpha);

    // Make sure the texture was loaded correctly
    if (pcPixels == nullptr)
    {
        throw std::runtime_error("unable to load texture");
    }

    // Check if the texture is valid
    assert(iTexWidth > 0);
    assert(iTexHeight > 0);

    // Create a struct with information about the texture
    tTextureInfo tTextureInfo = {};
    tTextureInfo.uiWidth = iTexWidth;
    tTextureInfo.uiHeight = iTexHeight;
    tTextureInfo.uiSize = iTexWidth * iTexHeight * 4; // we're using RGBA so 4 byte per pixel
    tTextureInfo.uiMipLevels = std::floor(std::log2(std::max(iTexWidth, iTexHeight))) + 1;

    // Create the texture object and return it
    return new cTexture(ppLogicalDevice, tTextureInfo, pcPixels);
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
    tSamplerInfo.maxAnisotropy = cPhysicalDevice::GetInstance()->GetMaxAnisotropy();

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

VkSampler cTextureHandler::GetSampler()
{
    assert(poTextureSampler != VK_NULL_HANDLE);

    return poTextureSampler;
}
