#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/geometry/BufferHelper.hpp>
#include <vulkan/util/ImageHelper.hpp>
#include <vulkan/texture/TextureHelper.hpp>
#include <vulkan/texture/TextureInfo.hpp>
#include "TextureSampler.hpp"

// Class representing a texture
class cTexture
{
private:
    // Logical device where this texture is loaded
    cLogicalDevice* ppLogicalDevice;

    string psFilePath;

    // Information about this texture
    tTextureInfo ptTextureInfo;

    stbi_uc* ppcPixels = nullptr;

    // Texture image, memory and image view
    VkImage poTextureImage = VK_NULL_HANDLE;
    VkDeviceMemory poTextureImageMemory = VK_NULL_HANDLE;
    VkImageView poTextureImageView = VK_NULL_HANDLE;

    cTextureSampler* ppSampler;

    bool pbLoaded = false;

public:
    cTexture(cLogicalDevice* pLogicalDevice,
             const string& sFilePath,
             cTextureSampler* pSampler);
    ~cTexture();

    void LoadIntoRAM();
    void LoadIntoGPU();
    void UnloadFromRAM();
    void UnloadFromGPU();

    bool IsLoaded();

    // Returns the information about this texture
    tTextureInfo GetTextureInfo();
    // Returns the image view for this texture
    VkImageView& GetView();

    VkSampler& GetSampler();
};

cTexture::cTexture(cLogicalDevice* pLogicalDevice,
                   const string& sFilePath,
                   cTextureSampler* pSampler)
{
    assert(pLogicalDevice != nullptr);                              // logical device must exist
    assert(sFilePath.length() > 0);                                 // file path must have a value
    assert(pSampler != nullptr);                                    // sampler must exist

    // Store the logical device and texture info
    ppLogicalDevice = pLogicalDevice;
    psFilePath = sFilePath;
    ppSampler = pSampler;
}

cTexture::~cTexture()
{
    if (ppcPixels != nullptr)
    {
        UnloadFromRAM();
    }
    if (poTextureImage != VK_NULL_HANDLE)
    {
        UnloadFromGPU();
    }
}

tTextureInfo cTexture::GetTextureInfo()
{
    return ptTextureInfo;
}

VkImageView& cTexture::GetView()
{
    assert(poTextureImageView != VK_NULL_HANDLE);

    return poTextureImageView;
}

VkSampler& cTexture::GetSampler()
{
    return ppSampler->GetSampler();
}

void cTexture::LoadIntoRAM()
{
    assert(psFilePath.length() > 0);
    assert(ppcPixels == nullptr);

    // Load the pixel data and image size
    int iTexWidth, iTexHeight;
    ppcPixels = stbi_load(psFilePath.c_str(), &iTexWidth, &iTexHeight, nullptr, STBI_rgb_alpha);

    // Make sure the texture was loaded correctly
    if (ppcPixels == nullptr)
    {
        throw std::runtime_error(cFormatter() << "unable to load texture '" << psFilePath << "'");
    }

    // Check if the texture is valid
    assert(iTexWidth > 0);
    assert(iTexHeight > 0);

    // Fill the struct with information about the texture
    ptTextureInfo.uiWidth = iTexWidth;
    ptTextureInfo.uiHeight = iTexHeight;
    ptTextureInfo.uiSize = iTexWidth * iTexHeight * 4; // we're using RGBA so 4 byte per pixel //-V112
    ptTextureInfo.uiMipLevels = (uint) std::floor(std::log2(std::max(iTexWidth, iTexHeight))) + 1;
}

void cTexture::LoadIntoGPU()
{
    assert(ppcPixels != nullptr);
    assert(poTextureImage == VK_NULL_HANDLE);
    assert(poTextureImageMemory == VK_NULL_HANDLE);
    assert(poTextureImageView == VK_NULL_HANDLE);

    // Create the image
    // We want to use the SRGB format and optimal tiling
    cImageHelper::CreateImage(ptTextureInfo.uiWidth, ptTextureInfo.uiHeight,
                              VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                              VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                              poTextureImage, poTextureImageMemory, ppLogicalDevice);

    // Copy the texture (pixels) to the created image
    cTextureHelper::CopyTextureToImage(ppLogicalDevice, ppcPixels, ptTextureInfo, poTextureImage);

    // Create an image view for this image
    cImageHelper::CreateImageView(poTextureImage,
                                  VK_FORMAT_R8G8B8A8_SRGB,
                                  ppLogicalDevice, &poTextureImageView,
                                  VK_IMAGE_ASPECT_COLOR_BIT);

    pbLoaded = true;

    ENGINE_LOG("Loaded texture " << psFilePath
                                 << " (" << ptTextureInfo.uiWidth << "x" << ptTextureInfo.uiHeight << ")");
}

void cTexture::UnloadFromRAM()
{
    assert(ppcPixels != nullptr);

    stbi_image_free(ppcPixels);
    ppcPixels = nullptr;
}

void cTexture::UnloadFromGPU()
{
    assert(poTextureImage != VK_NULL_HANDLE);
    assert(poTextureImageMemory != VK_NULL_HANDLE);
    assert(poTextureImageView != VK_NULL_HANDLE);

    // Destroy the image view and image, and free the image memory
    vkDestroyImageView(ppLogicalDevice->GetDevice(), poTextureImageView, nullptr);
    vkDestroyImage(ppLogicalDevice->GetDevice(), poTextureImage, nullptr);
    ppLogicalDevice->FreeMemory(poTextureImageMemory, nullptr);

    poTextureImage = VK_NULL_HANDLE;
    poTextureImageMemory = VK_NULL_HANDLE;
    poTextureImageView = VK_NULL_HANDLE;
}

bool cTexture::IsLoaded()
{
    return pbLoaded;
}
