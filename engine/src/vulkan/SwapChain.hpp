#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/Window.hpp>
#include <vulkan/PhysicalDevice.hpp>
#include <vulkan/LogicalDevice.hpp>

class cSwapChain
{
private:
    cLogicalDevice* ppLogicalDevice;

    VkSwapchainKHR poSwapChain;

    std::vector<VkImage> paoSwapChainImages;
    std::vector<VkImageView> paoSwapChainImageViews;

public:
    VkExtent2D ptSwapChainExtent;
    VkFormat peSwapChainImageFormat;

    cSwapChain(cPhysicalDevice* pPhysicalDevice,
               cLogicalDevice* pLogicalDevice,
               cWindow* pWindow,
               cSurface* pSurface);
    ~cSwapChain(void);

private:
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& atAvailableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& atAvailablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& tCapabilities, cWindow* pWindow);
    uint ChooseSwapImageCount(const VkSurfaceCapabilitiesKHR& tCapabilities);

    VkSwapchainCreateInfoKHR GetSwapChainCreateInfo(VkSurfaceFormatKHR& tSurfaceFormat,
                                                    VkPresentModeKHR ePresentMode,
                                                    VkExtent2D& tExtent,
                                                    uint uiImageCount,
                                                    cSurface* pSurface,
                                                    cPhysicalDevice* pPhysicalDevice,
                                                    tSwapChainSupportDetails& tSwapChainSupport);

    void CreateSwapChain(cPhysicalDevice* pPhysicalDevice, cWindow* pWindow, cSurface* pSurface);
    void CreateImageViews(void);
};

cSwapChain::cSwapChain(cPhysicalDevice* pPhysicalDevice,
                       cLogicalDevice* pLogicalDevice,
                       cWindow* pWindow,
                       cSurface* pSurface)
{
    ppLogicalDevice = pLogicalDevice;

    CreateSwapChain(pPhysicalDevice, pWindow, pSurface);
    CreateImageViews();
}

cSwapChain::~cSwapChain()
{
    // Destroy all the image views
    VkDevice& oDevice = ppLogicalDevice->GetDevice();
    for (VkImageView imageView : paoSwapChainImageViews)
    {
        vkDestroyImageView(oDevice, imageView, nullptr);
    }

    // Destroy the swap chain
    vkDestroySwapchainKHR(ppLogicalDevice->GetDevice(), poSwapChain, NULL);
}

VkSurfaceFormatKHR cSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& atAvailableFormats)
{
    // Try to Find a 32 bit SRGB format and return it
    for (VkSurfaceFormatKHR tAvailableFormat : atAvailableFormats)
    {
        if (tAvailableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            tAvailableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return tAvailableFormat;
        }
    }

    // If none are found, just return the first available format
    return atAvailableFormats[0];
}

VkPresentModeKHR cSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& atAvailablePresentModes)
{
    // If VK_PRESENT_MODE_MAILBOX_KHR is available, we want to use that
    for (VkPresentModeKHR tAvailablePresentMode : atAvailablePresentModes)
    {
        if (tAvailablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return tAvailablePresentMode;
        }
    }

    // Otherwise, use VK_PRESENT_MODE_FIFO_KHR, which is always available
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D cSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& tCapabilities, cWindow* pWindow)
{
    if (tCapabilities.currentExtent.width != UINT32_MAX)
    {
        // By default the window manager sets currentExtent to the exact window resolution, so we can use that
        return tCapabilities.currentExtent;
    }
    else
    {
        // If currentExtent is not present, use our window size clamped to the bounds of the capabilities

        VkExtent2D actualExtent = {pWindow->WIDTH, pWindow->HEIGHT};

        actualExtent.width = std::max(tCapabilities.minImageExtent.width,
                                      std::min(tCapabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(tCapabilities.minImageExtent.height,
                                       std::min(tCapabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

uint cSwapChain::ChooseSwapImageCount(const VkSurfaceCapabilitiesKHR& tCapabilities)
{
    // Number of images in the swap chain. It's recommended to have at least 1 more than the minimum
    // to prevent having to wait on the driver when rendering the next image
    uint uiImageCount = tCapabilities.minImageCount + 1;

    // Make sure we don't exceed the max number of images in the swap chain. A max of 0 means there is no maximum
    if (tCapabilities.maxImageCount > 0 && uiImageCount > tCapabilities.maxImageCount)
    {
        uiImageCount = tCapabilities.maxImageCount;
    }

    return uiImageCount;
}

VkSwapchainCreateInfoKHR cSwapChain::GetSwapChainCreateInfo(VkSurfaceFormatKHR& tSurfaceFormat,
                                                            VkPresentModeKHR ePresentMode,
                                                            VkExtent2D& tExtent,
                                                            uint uiImageCount,
                                                            cSurface* pSurface,
                                                            cPhysicalDevice* pPhysicalDevice,
                                                            tSwapChainSupportDetails& tSwapChainSupport)
{
    // Struct with information for creating the swap chain
    VkSwapchainCreateInfoKHR tCreateInfo{};
    tCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

    // Specify which surface to tie the swap chain to
    tCreateInfo.surface = pSurface->GetSurface();

    // Specify the details of the swap chain images
    tCreateInfo.minImageCount = uiImageCount;
    tCreateInfo.imageFormat = tSurfaceFormat.format;
    tCreateInfo.imageColorSpace = tSurfaceFormat.colorSpace;
    tCreateInfo.imageExtent = tExtent;
    tCreateInfo.imageArrayLayers = 1; // Amount of layers per image. Only used for stereo 3D
    tCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Get the indices of the queue families that we want to use
    tQueueFamilyIndices indices = pPhysicalDevice->FindQueueFamilies();
    uint queueFamilyIndices[] = {indices.oulGraphicsFamily.value(), indices.oulPresentFamily.value()};

    if (indices.oulGraphicsFamily != indices.oulPresentFamily)
    {
        // If the graphics queue family isn't the same as the presentation queue family
        // we want to ue concurrent mode and set the indices
        tCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        tCreateInfo.queueFamilyIndexCount = 2;
        tCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        // If the two queue families are the same, use exclusive mode. Here we don't
        // have to specify the the queue families in advance
        tCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        tCreateInfo.queueFamilyIndexCount = 0; // Optional
        tCreateInfo.pQueueFamilyIndices = NULL; // Optional
    }

    // Here we can define a transform that should be applied to the images
    // We don't want this, so set to the default
    tCreateInfo.preTransform = tSwapChainSupport.tCapabilities.currentTransform;

    // Can be used if we want our window to be translucent
    // We don't want this, so just set to OPAQUE
    tCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    // Set the presentation mode. Setting clipped to true means we don't care about pixels that are obscured
    tCreateInfo.presentMode = ePresentMode;
    tCreateInfo.clipped = VK_TRUE;

    // Needs to be used when re-creating the swap chain, for example when the window gets resized
    // For now, just setting this to NULL
    tCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    return tCreateInfo;
}

void cSwapChain::CreateSwapChain(cPhysicalDevice* pPhysicalDevice, cWindow* pWindow, cSurface* pSurface)
{
    VkDevice& oDevice = ppLogicalDevice->GetDevice();

    // Get the swap chain support details
    tSwapChainSupportDetails tSwapChainSupport = pPhysicalDevice->QuerySwapChainSupport();

    // Choose swap chain format, present mode, extend and image count
    VkSurfaceFormatKHR tSurfaceFormat = ChooseSwapSurfaceFormat(tSwapChainSupport.atFormats);
    VkPresentModeKHR ePresentMode = ChooseSwapPresentMode(tSwapChainSupport.atPresentModes);
    VkExtent2D tExtent = ChooseSwapExtent(tSwapChainSupport.tCapabilities, pWindow);
    uint uiImageCount = ChooseSwapImageCount(tSwapChainSupport.tCapabilities);

    // Store the image format and extent for later use
    peSwapChainImageFormat = tSurfaceFormat.format;
    ptSwapChainExtent = tExtent;

    // Create the swap chain create info
    VkSwapchainCreateInfoKHR tCreateInfo = GetSwapChainCreateInfo(
            tSurfaceFormat, ePresentMode, tExtent, uiImageCount,
            pSurface, pPhysicalDevice, tSwapChainSupport
    );

    // Create the swap chain
    if (vkCreateSwapchainKHR(oDevice, &tCreateInfo, NULL, &poSwapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    // Get the swap chain image handles
    vkGetSwapchainImagesKHR(oDevice, poSwapChain, &uiImageCount, NULL);
    paoSwapChainImages.resize(uiImageCount);
    vkGetSwapchainImagesKHR(oDevice, poSwapChain, &uiImageCount, paoSwapChainImages.data());
}

void cSwapChain::CreateImageViews(void)
{
    VkDevice& oDevice = ppLogicalDevice->GetDevice();

    // Resize the image views list to fit all the swap chain images
    paoSwapChainImageViews.resize(paoSwapChainImages.size());

    for (size_t i = 0; i < paoSwapChainImages.size(); i++)
    {
        // Struct with information for creating an image view
        VkImageViewCreateInfo tCreateInfo{};
        tCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

        // Set the swap chain image
        tCreateInfo.image = paoSwapChainImages[i];

        // Set the type to 2D and the format to our swap chain image format
        tCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        tCreateInfo.format = peSwapChainImageFormat;

        // Allows you to swizzle color channels around. We're just using the defaults
        tCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        tCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        tCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        tCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // Allows you to specify the purpose of this image. We just want simple color targets
        tCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        tCreateInfo.subresourceRange.baseMipLevel = 0;
        tCreateInfo.subresourceRange.levelCount = 1;
        tCreateInfo.subresourceRange.baseArrayLayer = 0;
        tCreateInfo.subresourceRange.layerCount = 1;

        // Create the image view
        if (vkCreateImageView(oDevice, &tCreateInfo, nullptr, &paoSwapChainImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image view!");
        }
    }
}
