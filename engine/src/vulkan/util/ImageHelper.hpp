#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/PhysicalDevice.hpp>
#include <vulkan/geometry/BufferHelper.hpp>

class cImageHelper
{
public:
    static void CreateImage(uint uiWidth,
                            uint uiHeight,
                            VkFormat eFormat,
                            VkImageTiling eTiling,
                            VkImageUsageFlags uiUsage,
                            VkMemoryPropertyFlags uiProperties,
                            VkImage& oImage,
                            VkDeviceMemory& oImageMemory,
                            cLogicalDevice* pLogicalDevice);
    static void CreateImageView(VkImage& oImage,
                                VkFormat eFormat,
                                cLogicalDevice* pLogicalDevice,
                                VkImageView* pImageView,
                                VkImageAspectFlags uiAspectFlags);

    static VkFormat FindDepthFormat(void);
    static bool HasStencilComponent(VkFormat eFormat);
    static VkFormat FindSupportedFormat(const std::vector<VkFormat>& aeCandidates,
                                        VkImageTiling eTiling,
                                        VkFormatFeatureFlags uiFeatures);
};

void cImageHelper::CreateImage(uint uiWidth,
                               uint uiHeight,
                               VkFormat eFormat,
                               VkImageTiling eTiling,
                               VkImageUsageFlags uiUsage,
                               VkMemoryPropertyFlags uiProperties,
                               VkImage& oImage,
                               VkDeviceMemory& oImageMemory,
                               cLogicalDevice* pLogicalDevice)
{
    assert(uiWidth > 0 && uiHeight > 0);
    assert(pLogicalDevice != nullptr);

    VkImageCreateInfo tImageInfo = {};
    tImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    tImageInfo.imageType = VK_IMAGE_TYPE_2D;
    tImageInfo.extent.width = uiWidth;
    tImageInfo.extent.height = uiHeight;
    tImageInfo.extent.depth = 1;
    tImageInfo.mipLevels = 1;
    tImageInfo.arrayLayers = 1;
    tImageInfo.format = eFormat;
    tImageInfo.tiling = eTiling;
    tImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    tImageInfo.usage = uiUsage;
    tImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    tImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (!pLogicalDevice->CreateImage(&tImageInfo, nullptr, &oImage))
    {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    pLogicalDevice->GetImageMemoryRequirements(oImage, &memRequirements);

    VkMemoryAllocateInfo tAllocInfo = {};
    tAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    tAllocInfo.allocationSize = memRequirements.size;
    tAllocInfo.memoryTypeIndex = cBufferHelper::FindMemoryType(memRequirements.memoryTypeBits, uiProperties);

    if (!pLogicalDevice->AllocateMemory(&tAllocInfo, nullptr, &oImageMemory))
    {
        throw std::runtime_error("failed to allocate image memory!");
    }

    pLogicalDevice->BindImageMemory(oImage, oImageMemory, 0);
}

VkFormat cImageHelper::FindDepthFormat(void)
{
    return FindSupportedFormat(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

bool cImageHelper::HasStencilComponent(VkFormat eFormat)
{
    return eFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || eFormat == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkFormat cImageHelper::FindSupportedFormat(const std::vector<VkFormat>& aeCandidates,
                                           VkImageTiling eTiling,
                                           VkFormatFeatureFlags uiFeatures)
{
    assert(aeCandidates.size() > 0); // must have at least one candidate

    for (VkFormat eFormat : aeCandidates)
    {
        VkFormatProperties tProps;
        cPhysicalDevice::GetInstance()->GetDeviceFormatProperties(eFormat, &tProps);

        if (eTiling == VK_IMAGE_TILING_LINEAR && (tProps.linearTilingFeatures & uiFeatures) == uiFeatures)
        {
            return eFormat;
        }
        else if (eTiling == VK_IMAGE_TILING_OPTIMAL && (tProps.optimalTilingFeatures & uiFeatures) == uiFeatures)
        {
            return eFormat;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

void cImageHelper::CreateImageView(VkImage& oImage,
                                   VkFormat eFormat,
                                   cLogicalDevice* pLogicalDevice,
                                   VkImageView* pImageView,
                                   VkImageAspectFlags uiAspectFlags)
{
    assert(oImage != VK_NULL_HANDLE);
    assert(pLogicalDevice != nullptr);
    assert(pImageView != nullptr);

    // Struct with information for creating an image view
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    // Set the type to 2D
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

    // Set the image and format
    viewInfo.image = oImage;
    viewInfo.format = eFormat;

    // Allows you to specify the purpose of this image. We're just setting the aspect flags
    viewInfo.subresourceRange.aspectMask = uiAspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    // Create the image view
    if (!pLogicalDevice->CreateImageView(&viewInfo, nullptr, pImageView))
    {
        throw std::runtime_error("failed to create texture image view!");
    }
}
