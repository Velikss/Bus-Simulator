#pragma once

#include <pch.hpp>
#include <vulkan/geometry/BufferHelper.hpp>
#include <vulkan/ImageHelper.hpp>
#include <vulkan/texture/TextureInfo.hpp>
#include <vulkan/texture/TextureHelper.hpp>

class cFont
{
private:
    cLogicalDevice* ppLogicalDevice;

    VkImage poFontImage = VK_NULL_HANDLE;
    VkDeviceMemory poFontImageMemory = VK_NULL_HANDLE;

public:
    VkImageView poFontImageView = VK_NULL_HANDLE;
    VkSampler poFontImageSampler = VK_NULL_HANDLE;

    cFont(cLogicalDevice* pLogicalDevice, uint uiFontWidth, uint uiFontHeight, byte* abFont24pixels);
    ~cFont();

private:
    void CreateFontImage(cLogicalDevice* pLogicalDevice, uint uiFontWidth, uint uiFontHeight, byte* abFont24pixels);
    void CreateTextureSampler(cLogicalDevice* pLogicalDevice);
};

cFont::cFont(cLogicalDevice* pLogicalDevice, uint uiFontWidth, uint uiFontHeight, byte* abFont24pixels)
{
    assert(pLogicalDevice != nullptr);           // logical device must exist
    assert(uiFontWidth > 0 && uiFontHeight > 0); // font must have a valid size
    assert(abFont24pixels != nullptr);           // font data must exist

    ppLogicalDevice = pLogicalDevice;

    CreateFontImage(pLogicalDevice, uiFontWidth, uiFontHeight, abFont24pixels);
    CreateTextureSampler(pLogicalDevice);
}

cFont::~cFont()
{
    vkDestroyImageView(ppLogicalDevice->GetDevice(), poFontImageView, nullptr);
    vkDestroyImage(ppLogicalDevice->GetDevice(), poFontImage, nullptr);
    ppLogicalDevice->FreeMemory(poFontImageMemory, nullptr);
    ppLogicalDevice->DestroySampler(poFontImageSampler, nullptr);
}

void cFont::CreateFontImage(cLogicalDevice* pLogicalDevice, uint uiFontWidth, uint uiFontHeight, byte* abFont24pixels)
{
    tTextureInfo tTextureInfo = {};
    tTextureInfo.uiWidth = uiFontWidth;
    tTextureInfo.uiHeight = uiFontHeight;
    tTextureInfo.uiSize = uiFontWidth * uiFontHeight;
    tTextureInfo.uiMipLevels = 1;

    // Create the font texture image
    // We want to use the SRGB format and optimal tiling
    cImageHelper::CreateImage(tTextureInfo.uiWidth, tTextureInfo.uiHeight,
                              VK_FORMAT_R8_UNORM, VK_IMAGE_TILING_OPTIMAL,
                              VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                              poFontImage, poFontImageMemory, pLogicalDevice);

    // Copy the texture (pixels) to the created image
    cTextureHelper::CopyTextureToImage(pLogicalDevice, abFont24pixels, tTextureInfo, poFontImage);

    // Create an image view for this image
    cImageHelper::CreateImageView(poFontImage,
                                  VK_FORMAT_R8_UNORM,
                                  pLogicalDevice, &poFontImageView,
                                  VK_IMAGE_ASPECT_COLOR_BIT);
}

void cFont::CreateTextureSampler(cLogicalDevice* pLogicalDevice)
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
    tSamplerInfo.anisotropyEnable = VK_FALSE;

    // When sampling outside the image, what color to use
    // irrelevant when using REPEAT addressing mode
    tSamplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

    // When true,  UV coordinates range from 0 to textureWidth and 0 to textureHeight
    // When false, UV coordinates range from 0 to 1
    tSamplerInfo.unnormalizedCoordinates = VK_FALSE;

    // Allows for comparing texel values to a value and using the result of the comparison
    tSamplerInfo.compareEnable = VK_FALSE;
    tSamplerInfo.compareOp = VK_COMPARE_OP_NEVER;

    // Configure mipmapping. Currently disabled
    tSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    tSamplerInfo.mipLodBias = 0.0f;
    tSamplerInfo.minLod = 0.0f;
    tSamplerInfo.maxLod = 1.0f;

    if (!pLogicalDevice->CreateSampler(&tSamplerInfo, nullptr, &poFontImageSampler))
    {
        throw std::runtime_error("failed to create texture sampler!");
    }
}
