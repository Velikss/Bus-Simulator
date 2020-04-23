#pragma once

#include <pch.hpp>
#include <vulkan/LogicalDevice.hpp>
#include "vulkan/buffer/BufferHelper.hpp"
#include "ImageHelper.hpp"
#include "CommandHelper.hpp"

class cTextureHandler
{
private:
    cLogicalDevice* ppLogicalDevice;

    VkBuffer poStagingBuffer;
    VkDeviceMemory poStagingBufferMemory;

    VkImage poTextureImage;
    VkDeviceMemory poTextureImageMemory;

public:
    static VkImageView poTextureImageView;
    static VkSampler poTextureSampler;

    cTextureHandler(cLogicalDevice* pLogicalDevice);
    ~cTextureHandler(void);

private:
    void CreateTextureImage(void);
    void CreateTextureImageView(void);
    void CreateTextureSampler(void);

    static void TransitionImageLayout(VkImage& oImage,
                                      VkFormat eFormat,
                                      VkImageLayout eOldLayout,
                                      VkImageLayout eNewLayout,
                                      cLogicalDevice* pLogicalDevice);
    static void CopyBufferToImage(VkBuffer& oBuffer,
                                  VkImage& oImage,
                                  uint uiWidth,
                                  uint uiHeight,
                                  cLogicalDevice* pLogicalDevice);
};

VkImageView cTextureHandler::poTextureImageView = VK_NULL_HANDLE;
VkSampler cTextureHandler::poTextureSampler = VK_NULL_HANDLE;

cTextureHandler::cTextureHandler(cLogicalDevice* pLogicalDevice)
{
    ppLogicalDevice = pLogicalDevice;

    CreateTextureImage();
    CreateTextureImageView();
    CreateTextureSampler();
}

cTextureHandler::~cTextureHandler(void)
{
    ppLogicalDevice->DestroySampler(poTextureSampler, nullptr);
    vkDestroyImageView(ppLogicalDevice->GetDevice(), poTextureImageView, nullptr);
    vkDestroyImage(ppLogicalDevice->GetDevice(), poTextureImage, nullptr);
    ppLogicalDevice->FreeMemory(poTextureImageMemory, nullptr);
}

void cTextureHandler::CreateTextureImage(void)
{
    // Load the pixel data and image size
    int iTexWidth, iTexHeight, iTexChannels;
    stbi_uc* pcPixels = stbi_load("resources/chalet.jpg", &iTexWidth, &iTexHeight, &iTexChannels, STBI_rgb_alpha);
    VkDeviceSize ulImageSize = iTexWidth * iTexHeight * 4; // we're using RGBA so 4 byte per pixel

    if (!pcPixels)
    {
        throw std::runtime_error("failed to load texture image!");
    }

    // Create the stagingBuffer which is host visible and coherent
    cBufferHelper::CreateBuffer(ppLogicalDevice, ulImageSize,
                                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                poStagingBuffer, poStagingBufferMemory);

    // Copy the pixel data into the buffer
    void* pData;
    ppLogicalDevice->MapMemory(poStagingBufferMemory, 0, ulImageSize, 0, &pData);
    {
        memcpy(pData, pcPixels, ulImageSize);
    }
    ppLogicalDevice->UnmapMemory(poStagingBufferMemory);

    // Free the pixel data memory
    stbi_image_free(pcPixels);

    // Create the image
    cImageHelper::CreateImage(iTexWidth, iTexHeight, VK_FORMAT_R8G8B8A8_SRGB,
                              VK_IMAGE_TILING_OPTIMAL,
                              VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                              poTextureImage, poTextureImageMemory, ppLogicalDevice);

    // Transition the image to a TRANSFER_DST_OPTIMAL layout
    TransitionImageLayout(poTextureImage,
                          VK_FORMAT_R8G8B8A8_SRGB,
                          VK_IMAGE_LAYOUT_UNDEFINED, // cImageHelper::CreateImage returns an image with LAYOUT_UNDEFINED
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          ppLogicalDevice);

    // Copy the stagingBuffer to the image
    CopyBufferToImage(poStagingBuffer, poTextureImage, iTexWidth, iTexHeight, ppLogicalDevice);

    // Transition the image to a SHADER_READ_ONLY layout
    TransitionImageLayout(poTextureImage,
                          VK_FORMAT_R8G8B8A8_SRGB,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                          ppLogicalDevice);

    ppLogicalDevice->DestroyBuffer(poStagingBuffer, nullptr);
    ppLogicalDevice->FreeMemory(poStagingBufferMemory, nullptr);
}

void cTextureHandler::CreateTextureImageView(void)
{
    cImageHelper::CreateImageView(poTextureImage,
                                  VK_FORMAT_R8G8B8A8_SRGB,
                                  ppLogicalDevice, &poTextureImageView,
                                  VK_IMAGE_ASPECT_COLOR_BIT);
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

void cTextureHandler::TransitionImageLayout(VkImage& oImage,
                                            VkFormat eFormat,
                                            VkImageLayout eOldLayout,
                                            VkImageLayout eNewLayout,
                                            cLogicalDevice* pLogicalDevice)
{
    VkImageMemoryBarrier tBarrier = {};
    tBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    tBarrier.oldLayout = eOldLayout;
    tBarrier.newLayout = eNewLayout;

    tBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    tBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    tBarrier.image = oImage;

    tBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    tBarrier.subresourceRange.baseMipLevel = 0;
    tBarrier.subresourceRange.levelCount = 1;
    tBarrier.subresourceRange.baseArrayLayer = 0;
    tBarrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags uiSourceStage;
    VkPipelineStageFlags uiDestinationStage;

    if (eOldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        eNewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) // undefined -> transfer destination
    {
        tBarrier.srcAccessMask = 0;
        uiSourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

        tBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        uiDestinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (eOldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             eNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) // transfer destination -> shader readonly
    {
        tBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        uiSourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

        tBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        uiDestinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }

    VkCommandBuffer oCommandBuffer = cCommandHelper::BeginSingleTimeCommands(pLogicalDevice);
    {
        vkCmdPipelineBarrier(
                oCommandBuffer,
                uiSourceStage, uiDestinationStage,
                0,
                0, nullptr,
                0, nullptr,
                1, &tBarrier
        );
    }
    cCommandHelper::EndSingleTimeCommands(pLogicalDevice, oCommandBuffer);
}

void cTextureHandler::CopyBufferToImage(VkBuffer& oBuffer,
                                        VkImage& oImage,
                                        uint uiWidth,
                                        uint uiHeight,
                                        cLogicalDevice* pLogicalDevice)
{
    VkBufferImageCopy tRegion = {};

    // Specify how the pixels are laid out in memory
    // We don't have anything special, so just all zero's
    tRegion.bufferOffset = 0;
    tRegion.bufferRowLength = 0;
    tRegion.bufferImageHeight = 0;

    // Specify which subresource to use
    tRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    tRegion.imageSubresource.mipLevel = 0;
    tRegion.imageSubresource.baseArrayLayer = 0;
    tRegion.imageSubresource.layerCount = 1;

    // Specify which part of the image we want to copy
    tRegion.imageOffset = {0, 0, 0};
    tRegion.imageExtent = {uiWidth, uiHeight, 1};

    VkCommandBuffer oCommandBuffer = cCommandHelper::BeginSingleTimeCommands(pLogicalDevice);
    {
        vkCmdCopyBufferToImage(
                oCommandBuffer, oBuffer, oImage,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &tRegion
        );
    }
    cCommandHelper::EndSingleTimeCommands(pLogicalDevice, oCommandBuffer);
}
