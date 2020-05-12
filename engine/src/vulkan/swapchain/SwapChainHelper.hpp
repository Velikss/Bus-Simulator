#pragma once

#include <pch.hpp>
#include <vulkan/vulkan_core.h>
#include "vulkan/util/ImageHelper.hpp"

struct tFrameBufferAttachment
{
    VkImage oImage = VK_NULL_HANDLE;
    VkDeviceMemory oMemory = VK_NULL_HANDLE;
    VkImageView oView = VK_NULL_HANDLE;
    VkFormat eFormat;
    VkAttachmentDescription tDescription;
};

class cSwapChainHelper
{
public:
    static void CreateAttachment(VkFormat eFormat,
                                 VkImageUsageFlagBits uiUsage,
                                 tFrameBufferAttachment* ptAttachment,
                                 cLogicalDevice* pLogicalDevice,
                                 VkExtent2D tSize);
};

void cSwapChainHelper::CreateAttachment(VkFormat eFormat,
                                        VkImageUsageFlagBits uiUsage,
                                        tFrameBufferAttachment* ptAttachment,
                                        cLogicalDevice* pLogicalDevice,
                                        VkExtent2D tSize)
{
    VkImageAspectFlags uiAspectMask = 0;
    VkImageLayout eImageLayout;

    ptAttachment->eFormat = eFormat;

    if (uiUsage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
    {
        uiAspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        eImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }
    if (uiUsage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        uiAspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        eImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    assert(uiAspectMask > 0);

    cImageHelper::CreateImage(tSize.width, tSize.height,
                              eFormat, VK_IMAGE_TILING_OPTIMAL,
                              uiUsage | VK_IMAGE_USAGE_SAMPLED_BIT,
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                              ptAttachment->oImage, ptAttachment->oMemory,
                              pLogicalDevice);

    cImageHelper::CreateImageView(ptAttachment->oImage, eFormat,
                                  pLogicalDevice, &ptAttachment->oView,
                                  uiAspectMask);

    ptAttachment->tDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    ptAttachment->tDescription.format = eFormat;

    ptAttachment->tDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    ptAttachment->tDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    ptAttachment->tDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    ptAttachment->tDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    ptAttachment->tDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ptAttachment->tDescription.finalLayout = eImageLayout;
    ptAttachment->tDescription.flags = 0;
}
