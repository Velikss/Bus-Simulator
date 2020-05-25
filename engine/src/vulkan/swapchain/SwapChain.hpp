#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/Window.hpp>
#include <vulkan/PhysicalDevice.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/util/ImageHelper.hpp>
#include <vulkan/swapchain/SwapChainHelper.hpp>
#include <util/Formatter.hpp>
#include <vulkan/texture/TextureHelper.hpp>

struct tOffScreenBuffer
{
    tFrameBufferAttachment ptPositionAttachment;
    tFrameBufferAttachment ptNormalsAttachment;
    tFrameBufferAttachment ptAlbedoAttachment;
    tFrameBufferAttachment ptDepthAttachment;
    tFrameBufferAttachment ptMaterialAttachment;

    VkFramebuffer poFramebuffer;

    VkSampler poSampler; // TODO: This probably doesn't belong here
};

struct tOverlayBuffer
{
    tFrameBufferAttachment ptColorAttachment;

    VkFramebuffer poFramebuffer;
};

class cSwapChain
{
private:
    cLogicalDevice* ppLogicalDevice;

    std::vector<VkImage> paoSwapChainImages;
    std::vector<VkImageView> paoSwapChainImageViews;
    std::vector<VkFramebuffer> paoSwapChainFramebuffers;

    tOffScreenBuffer ptOffScreenBuffer;
    tOverlayBuffer ptOverlayBuffer;

public:
    VkSwapchainKHR poSwapChain; // TODO: Remove public access

    VkExtent2D ptSwapChainExtent;
    VkFormat peSwapChainImageFormat;

    cSwapChain(cLogicalDevice* pLogicalDevice,
               cWindow* pWindow);
    ~cSwapChain(void);

    void CreateFramebuffers(VkRenderPass& oFinalRenderPass,
                            VkRenderPass& oOffScreenRenderPass,
                            VkRenderPass& oOverlayRenderPass);
    void CreateResources(void);

    uint GetFramebufferSize(void);
    VkFramebuffer& GetFramebuffer(uint index);
    tFrameBufferAttachment& GetAttachment(uint uiIndex);
    VkSampler& GetSampler();
    VkFramebuffer& GetOffScreenFramebuffer();
    tFrameBufferAttachment& GetOverlayAttachment();
    VkFramebuffer& GetOverlayFramebuffer();

    void AcquireNextImage(int64 ulTimeout,
                          VkSemaphore& oSemaphore,
                          VkFence& oFence,
                          uint* puiImageIndex);

private:
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& atAvailableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& atAvailablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& tCapabilities, cWindow* pWindow);
    uint ChooseSwapImageCount(const VkSurfaceCapabilitiesKHR& tCapabilities);

    VkSwapchainCreateInfoKHR GetSwapChainCreateInfo(VkSurfaceFormatKHR& tSurfaceFormat,
                                                    VkPresentModeKHR ePresentMode,
                                                    VkExtent2D& tExtent,
                                                    uint uiImageCount,
                                                    cWindow* pWindow,
                                                    cPhysicalDevice* pPhysicalDevice,
                                                    tSwapChainSupportDetails& tSwapChainSupport);

    void CreateSwapChain(cWindow* pWindow);
    void CreateImageViews(void);
};

cSwapChain::cSwapChain(cLogicalDevice* pLogicalDevice, cWindow* pWindow)
{
    ppLogicalDevice = pLogicalDevice;

    CreateSwapChain(pWindow);
    CreateImageViews();
}

cSwapChain::~cSwapChain()
{
    VkDevice& oDevice = ppLogicalDevice->GetDevice(); // TODO: Use internal cLogicalDevice methods

    std::array<tFrameBufferAttachment*, 6> aptAttachments = {
            &ptOffScreenBuffer.ptPositionAttachment,
            &ptOffScreenBuffer.ptNormalsAttachment,
            &ptOffScreenBuffer.ptAlbedoAttachment,
            &ptOffScreenBuffer.ptDepthAttachment,
            &ptOffScreenBuffer.ptMaterialAttachment,
            &ptOverlayBuffer.ptColorAttachment
    };
    for (tFrameBufferAttachment* pAttachment : aptAttachments)
    {
        vkDestroyImageView(oDevice, pAttachment->oView, nullptr);
        vkDestroyImage(oDevice, pAttachment->oImage, nullptr);
        ppLogicalDevice->FreeMemory(pAttachment->oMemory, nullptr);
    }

    ppLogicalDevice->DestroySampler(ptOffScreenBuffer.poSampler, nullptr);

    // Destroy all the framebuffers
    for (VkFramebuffer framebuffer : paoSwapChainFramebuffers)
    {
        vkDestroyFramebuffer(oDevice, framebuffer, nullptr);
    }
    vkDestroyFramebuffer(oDevice, ptOffScreenBuffer.poFramebuffer, nullptr);
    vkDestroyFramebuffer(oDevice, ptOverlayBuffer.poFramebuffer, nullptr);

    // Destroy all the image views
    for (VkImageView imageView : paoSwapChainImageViews)
    {
        vkDestroyImageView(oDevice, imageView, nullptr);
    }

    // Destroy the swap chain
    vkDestroySwapchainKHR(ppLogicalDevice->GetDevice(), poSwapChain, nullptr);
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

        VkExtent2D actualExtent = {WIDTH, HEIGHT};

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
                                                            cWindow* pWindow,
                                                            cPhysicalDevice* pPhysicalDevice,
                                                            tSwapChainSupportDetails& tSwapChainSupport)
{
    // Struct with information for creating the swap chain
    VkSwapchainCreateInfoKHR tCreateInfo{};
    tCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

    // Specify which surface to tie the swap chain to
    tCreateInfo.surface = pWindow->GetSurface();

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
        tCreateInfo.pQueueFamilyIndices = nullptr; // Optional
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
    // For now, just setting this to nullptr
    tCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    return tCreateInfo;
}

void cSwapChain::CreateSwapChain(cWindow* pWindow)
{
    cPhysicalDevice* pPhysicalDevice = cPhysicalDevice::GetInstance();
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
            pWindow, pPhysicalDevice, tSwapChainSupport
    );

    // Create the swap chain
    if (vkCreateSwapchainKHR(oDevice, &tCreateInfo, nullptr, &poSwapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    // Get the swap chain image handles
    vkGetSwapchainImagesKHR(oDevice, poSwapChain, &uiImageCount, nullptr);
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
        cImageHelper::CreateImageView(paoSwapChainImages[i],
                                      peSwapChainImageFormat, ppLogicalDevice,
                                      &paoSwapChainImageViews[i],
                                      VK_IMAGE_ASPECT_COLOR_BIT);
    }
}

void cSwapChain::CreateFramebuffers(VkRenderPass& oFinalRenderPass,
                                    VkRenderPass& oOffScreenRenderPass,
                                    VkRenderPass& oOverlayRenderPass)
{
    // Resize the framebuffers list to fit the image views
    paoSwapChainFramebuffers.resize(paoSwapChainImageViews.size());

    for (size_t i = 0; i < paoSwapChainImageViews.size(); i++)
    {

        // Struct with information about the framebuffer
        VkFramebufferCreateInfo tFramebufferInfo = {};
        tFramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

        // Render pass we want to use
        tFramebufferInfo.renderPass = oFinalRenderPass;

        // Which attachments want to bind
        tFramebufferInfo.attachmentCount = 1;
        tFramebufferInfo.pAttachments = &paoSwapChainImageViews[i];

        // With, height and number of layers
        tFramebufferInfo.width = ptSwapChainExtent.width;
        tFramebufferInfo.height = ptSwapChainExtent.height;
        tFramebufferInfo.layers = 1;

        // Create the framebuffer
        if (vkCreateFramebuffer(ppLogicalDevice->GetDevice(), &tFramebufferInfo, nullptr,
                                &paoSwapChainFramebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    std::array<VkImageView, 5> atAttachments;
    atAttachments[0] = ptOffScreenBuffer.ptPositionAttachment.oView;
    atAttachments[1] = ptOffScreenBuffer.ptNormalsAttachment.oView;
    atAttachments[2] = ptOffScreenBuffer.ptAlbedoAttachment.oView;
    atAttachments[3] = ptOffScreenBuffer.ptDepthAttachment.oView;
    atAttachments[4] = ptOffScreenBuffer.ptMaterialAttachment.oView;

    VkFramebufferCreateInfo tFramebufferInfo = {};
    tFramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    tFramebufferInfo.pNext = NULL;
    tFramebufferInfo.renderPass = oOffScreenRenderPass;
    tFramebufferInfo.pAttachments = atAttachments.data();
    tFramebufferInfo.attachmentCount = atAttachments.size();
    tFramebufferInfo.width = ptSwapChainExtent.width;
    tFramebufferInfo.height = ptSwapChainExtent.height;
    tFramebufferInfo.layers = 1;

    // Create the framebuffer
    if (vkCreateFramebuffer(ppLogicalDevice->GetDevice(), &tFramebufferInfo, nullptr,
                            &ptOffScreenBuffer.poFramebuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create offscreen framebuffer!");
    }

    VkFramebufferCreateInfo tOverlayBufferInfo = {};
    tOverlayBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    tOverlayBufferInfo.pNext = NULL;
    tOverlayBufferInfo.renderPass = oOverlayRenderPass;
    tOverlayBufferInfo.pAttachments = &ptOverlayBuffer.ptColorAttachment.oView;
    tOverlayBufferInfo.attachmentCount = 1;
    tOverlayBufferInfo.width = ptSwapChainExtent.width;
    tOverlayBufferInfo.height = ptSwapChainExtent.height;
    tOverlayBufferInfo.layers = 1;

    // Create the framebuffer
    if (vkCreateFramebuffer(ppLogicalDevice->GetDevice(), &tOverlayBufferInfo, nullptr,
                            &ptOverlayBuffer.poFramebuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create overlay framebuffer!");
    }
}

void cSwapChain::CreateResources(void) // TODO: This might belong somewhere else
{
    VkFormat eDepthFormat = cImageHelper::FindDepthFormat();
    cSwapChainHelper::CreateAttachment(eDepthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                       &ptOffScreenBuffer.ptDepthAttachment, ppLogicalDevice, ptSwapChainExtent);

    cSwapChainHelper::CreateAttachment(VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                       &ptOffScreenBuffer.ptPositionAttachment, ppLogicalDevice, ptSwapChainExtent);

    cSwapChainHelper::CreateAttachment(VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                       &ptOffScreenBuffer.ptNormalsAttachment, ppLogicalDevice, ptSwapChainExtent);

    cSwapChainHelper::CreateAttachment(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                       &ptOffScreenBuffer.ptAlbedoAttachment, ppLogicalDevice, ptSwapChainExtent);

    cSwapChainHelper::CreateAttachment(VK_FORMAT_R8G8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                       &ptOffScreenBuffer.ptMaterialAttachment, ppLogicalDevice, ptSwapChainExtent);

    cSwapChainHelper::CreateAttachment(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                       &ptOverlayBuffer.ptColorAttachment, ppLogicalDevice, ptSwapChainExtent);

    // Create sampler to sample from the color attachments
    VkSamplerCreateInfo tSampler = {};
    tSampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    tSampler.magFilter = VK_FILTER_NEAREST;
    tSampler.minFilter = VK_FILTER_NEAREST;
    tSampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    tSampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    tSampler.addressModeV = tSampler.addressModeU;
    tSampler.addressModeW = tSampler.addressModeU;
    tSampler.mipLodBias = 0.0f;
    tSampler.maxAnisotropy = 1.0f;
    tSampler.minLod = 0.0f;
    tSampler.maxLod = 1.0f;
    tSampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

    if (!ppLogicalDevice->CreateSampler(&tSampler, nullptr, &ptOffScreenBuffer.poSampler))
    {
        throw std::runtime_error("failed to create offscreen sampler!");
    }
}

uint cSwapChain::GetFramebufferSize(void)
{
    return paoSwapChainFramebuffers.size();
}

VkFramebuffer& cSwapChain::GetFramebuffer(uint index)
{
    return paoSwapChainFramebuffers[index];
}

void cSwapChain::AcquireNextImage(int64 ulTimeout,
                                  VkSemaphore& oSemaphore,
                                  VkFence& oFence,
                                  uint* pImageIndex)
{
    vkAcquireNextImageKHR(ppLogicalDevice->GetDevice(), poSwapChain, ulTimeout, oSemaphore, oFence, pImageIndex);
}

tFrameBufferAttachment& cSwapChain::GetAttachment(uint uiIndex)
{
    switch (uiIndex)
    {
        case 0:
            return ptOffScreenBuffer.ptPositionAttachment;
        case 1:
            return ptOffScreenBuffer.ptNormalsAttachment;
        case 2:
            return ptOffScreenBuffer.ptAlbedoAttachment;
        case 3:
            return ptOffScreenBuffer.ptDepthAttachment;
        case 4:
            return ptOffScreenBuffer.ptMaterialAttachment;
    }
    throw std::runtime_error(cFormatter() << "Cannot find attachment " << uiIndex);
}

VkSampler& cSwapChain::GetSampler()
{
    return ptOffScreenBuffer.poSampler;
}

VkFramebuffer& cSwapChain::GetOffScreenFramebuffer()
{
    return ptOffScreenBuffer.poFramebuffer;
}

tFrameBufferAttachment& cSwapChain::GetOverlayAttachment()
{
    return ptOverlayBuffer.ptColorAttachment;
}

VkFramebuffer& cSwapChain::GetOverlayFramebuffer()
{
    return ptOverlayBuffer.poFramebuffer;
}
