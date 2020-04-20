#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/SwapChain.hpp>

class cRenderPass
{
private:
    cLogicalDevice* ppLogicalDevice;

public:
    VkRenderPass poRenderPass;

    cRenderPass(cSwapChain* pSwapChain, cLogicalDevice* pLogicalDevice);
    ~cRenderPass(void);
};

cRenderPass::cRenderPass(cSwapChain* pSwapChain, cLogicalDevice* pLogicalDevice)
{
    ppLogicalDevice = pLogicalDevice;

    VkAttachmentDescription tColorAttachment = {};
    tColorAttachment.format = pSwapChain->peSwapChainImageFormat;
    tColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    tColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    tColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    tColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    tColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    tColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    tColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    tColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    tColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference tColorAttachmentRef = {};
    tColorAttachmentRef.attachment = 0;
    tColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription tSubpass = {};
    tSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    tSubpass.colorAttachmentCount = 1;
    tSubpass.pColorAttachments = &tColorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &tColorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &tSubpass;

    if (vkCreateRenderPass(pLogicalDevice->GetDevice(), &renderPassInfo, NULL, &poRenderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

cRenderPass::~cRenderPass(void)
{
    vkDestroyRenderPass(ppLogicalDevice->GetDevice(), poRenderPass, NULL);
}
