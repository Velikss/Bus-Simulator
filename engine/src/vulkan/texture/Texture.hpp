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

    // Information about this texture
    tTextureInfo ptTextureInfo;

    // Texture image, memory and image view
    VkImage poTextureImage = VK_NULL_HANDLE;
    VkDeviceMemory poTextureImageMemory = VK_NULL_HANDLE;
    VkImageView poTextureImageView = VK_NULL_HANDLE;

    cTextureSampler* ppSampler;

public:
    cTexture(cLogicalDevice* pLogicalDevice,
             tTextureInfo tTextureInfo,
             stbi_uc* pcPixels,
             const char* sName,
             cTextureSampler* pSampler);
    ~cTexture();

    // Returns the information about this texture
    tTextureInfo GetTextureInfo();
    // Returns the image view for this texture
    VkImageView GetView();

    VkSampler& GetSampler();
};

cTexture::cTexture(cLogicalDevice* pLogicalDevice,
                   tTextureInfo tTextureInfo,
                   stbi_uc* pcPixels,
                   const char* sName,
                   cTextureSampler* pSampler)
{
    assert(pLogicalDevice != nullptr);                              // logical device must exist
    assert(tTextureInfo.uiWidth > 0 && tTextureInfo.uiHeight > 0);  // texture must have a width and height
    assert(pcPixels != nullptr);                                    // pixels must exist
    assert(pSampler != nullptr);                                    // sampler must exist

    // Store the logical device and texture info
    ppLogicalDevice = pLogicalDevice;
    ptTextureInfo = tTextureInfo;
    ppSampler = pSampler;

    // Create the image
    // We want to use the SRGB format and optimal tiling
    cImageHelper::CreateImage(tTextureInfo.uiWidth, tTextureInfo.uiHeight,
                              VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                              VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                              poTextureImage, poTextureImageMemory, pLogicalDevice);

    // Copy the texture (pixels) to the created image
    cTextureHelper::CopyTextureToImage(pLogicalDevice, pcPixels, tTextureInfo, poTextureImage);

    // Create an image view for this image
    cImageHelper::CreateImageView(poTextureImage,
                                  VK_FORMAT_R8G8B8A8_SRGB,
                                  pLogicalDevice, &poTextureImageView,
                                  VK_IMAGE_ASPECT_COLOR_BIT);

    ENGINE_LOG("Loaded texture " << sName << " (" << tTextureInfo.uiWidth << "x" << tTextureInfo.uiHeight << ")");
}

cTexture::~cTexture()
{
    // Destroy the image view and image, and free the image memory
    vkDestroyImageView(ppLogicalDevice->GetDevice(), poTextureImageView, nullptr);
    vkDestroyImage(ppLogicalDevice->GetDevice(), poTextureImage, nullptr);
    ppLogicalDevice->FreeMemory(poTextureImageMemory, nullptr);
}

tTextureInfo cTexture::GetTextureInfo()
{
    return ptTextureInfo;
}

VkImageView cTexture::GetView()
{
    assert(poTextureImageView != VK_NULL_HANDLE);

    return poTextureImageView;
}

VkSampler& cTexture::GetSampler()
{
    return ppSampler->GetSampler();
}
