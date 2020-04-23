#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/CommandHelper.hpp>
#include <vulkan/texture/TextureInfo.hpp>

class cTextureHelper
{
public:
    static void CopyTextureToImage(cLogicalDevice* pLogicalDevice,
                                   stbi_uc* pcPixels,
                                   tTextureInfo tTextureInfo,
                                   VkImage& oImage);

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

void cTextureHelper::CopyTextureToImage(cLogicalDevice* pLogicalDevice,
                                        stbi_uc* pcPixels,
                                        tTextureInfo tTextureInfo,
                                        VkImage& oImage)
{

    assert(pLogicalDevice != nullptr);  // device must exist
    assert(oImage != VK_NULL_HANDLE);   // texture image must be created first
    assert(tTextureInfo.uiSize > 0);    // texture must have a size
    assert(pcPixels != nullptr);        // pixel data must exist
    assert(tTextureInfo.uiWidth > 0 && tTextureInfo.uiHeight > 0); // texture must have a width and height

    VkDeviceSize uiImageSize = tTextureInfo.uiSize;

    // Create the stagingBuffer which is host visible and coherent
    VkBuffer oStagingBuffer;
    VkDeviceMemory oStagingBufferMemory;
    cBufferHelper::CreateBuffer(pLogicalDevice, uiImageSize,
                                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                oStagingBuffer, oStagingBufferMemory);

    // Copy the pixel data into the buffer
    void* pData;
    pLogicalDevice->MapMemory(oStagingBufferMemory, 0, uiImageSize, 0, &pData);
    {
        memcpy(pData, pcPixels, uiImageSize);
    }
    pLogicalDevice->UnmapMemory(oStagingBufferMemory);

    // Transition the image to a TRANSFER_DST_OPTIMAL layout
    cTextureHelper::TransitionImageLayout(oImage,
                                          VK_FORMAT_R8G8B8A8_SRGB,
                                          VK_IMAGE_LAYOUT_UNDEFINED,
                                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                          pLogicalDevice);

    // Copy the stagingBuffer to the image
    cTextureHelper::CopyBufferToImage(oStagingBuffer, oImage,
                                      tTextureInfo.uiWidth, tTextureInfo.uiHeight,
                                      pLogicalDevice);

    // Transition the image to a SHADER_READ_ONLY layout
    cTextureHelper::TransitionImageLayout(oImage,
                                          VK_FORMAT_R8G8B8A8_SRGB,
                                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                          pLogicalDevice);

    // Clean up the stagingBuffer since we don't need it anymore
    pLogicalDevice->DestroyBuffer(oStagingBuffer, nullptr);
    pLogicalDevice->FreeMemory(oStagingBufferMemory, nullptr);
}

void cTextureHelper::TransitionImageLayout(VkImage& oImage,
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

void cTextureHelper::CopyBufferToImage(VkBuffer& oBuffer,
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
