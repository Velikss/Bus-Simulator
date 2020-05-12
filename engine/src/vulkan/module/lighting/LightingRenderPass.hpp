#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/SwapChain.hpp>
#include <vulkan/renderpass/RenderPass.hpp>
#include <vulkan/SwapChain.hpp>

class cLightingRenderPass : public cRenderPass
{
public:
    cLightingRenderPass(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain);

private:
    void GetColorAttachment(VkAttachmentDescription& tColorAttachment, cSwapChain* pSwapChain);
};

cLightingRenderPass::cLightingRenderPass(cLogicalDevice* pLogicalDevice,
                                         cSwapChain* pSwapChain) : cRenderPass(pLogicalDevice)
{
    // Struct with information about the color attachment
    VkAttachmentDescription tColorAttachment = {};
    GetColorAttachment(tColorAttachment, pSwapChain);

    // Struct which references the color attachment we just described
    VkAttachmentReference tColorAttachmentRef = {};
    tColorAttachmentRef.attachment = 0; // index of the color attachment
    tColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Struct describing the subpass
    VkSubpassDescription tSubpass = {};
    tSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    tSubpass.colorAttachmentCount = 1;
    tSubpass.pColorAttachments = &tColorAttachmentRef;
    tSubpass.pDepthStencilAttachment = nullptr;

    // TODO: Comments
    VkSubpassDependency tDependency = {};
    tDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    tDependency.dstSubpass = 0;

    tDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    tDependency.srcAccessMask = 0;

    tDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    tDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // Struct with information about the render pass
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    // Set the attachments
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &tColorAttachment;

    // Set the subpasses and dependencies
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &tSubpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &tDependency;

    // Create the render pass
    if (vkCreateRenderPass(pLogicalDevice->GetDevice(), &renderPassInfo, nullptr, &poRenderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

void cLightingRenderPass::GetColorAttachment(VkAttachmentDescription& tColorAttachment, cSwapChain* pSwapChain)
{
    // Set the format to the format of our swap chain images
    tColorAttachment.format = pSwapChain->peSwapChainImageFormat;

    // We're not using multisampling, so stick to 1 sample
    tColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    // Determine what to do with the data before and after rendering
    // We want to see the rendered image on screen, so we want to
    // go with the store operations here
    tColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    tColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    // Same as previous, but for the stencil data instead of color data
    // We don't use the stencil buffer, so we don't care
    tColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    tColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    // Layout of the pixels in memory
    tColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    tColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
}
