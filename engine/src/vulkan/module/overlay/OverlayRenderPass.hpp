#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/swapchain/SwapChain.hpp>
#include <vulkan/renderpass/RenderPass.hpp>

class cOverlayRenderPass : public cRenderPass
{
private:
    cSwapChain* ppSwapChain;

public:
    cOverlayRenderPass(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain);

protected:
    void CreateRenderPass();
};

cOverlayRenderPass::cOverlayRenderPass(cLogicalDevice* pLogicalDevice,
                                       cSwapChain* pSwapChain) : cRenderPass(pLogicalDevice)
{
    assert(pSwapChain != nullptr);

    ppSwapChain = pSwapChain;
    CreateRenderPass();
}

void cOverlayRenderPass::CreateRenderPass()
{
    VkAttachmentDescription tAttachments[1] = {};

    // Color attachment
    tAttachments[0].format = ppSwapChain->peSwapChainImageFormat;
    tAttachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    // Don't clear the framebuffer
    tAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    tAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    tAttachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    tAttachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    tAttachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    tAttachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference tColorReference = {};
    tColorReference.attachment = 0;
    tColorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // Use subpass dependencies for image layout transitions
    VkSubpassDependency tSubpassDependencies[2] = {};

    // Transition from final to initial (VK_SUBPASS_EXTERNAL refers to all commmands executed outside of the actual renderpass)
    tSubpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    tSubpassDependencies[0].dstSubpass = 0;
    tSubpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    tSubpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    tSubpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    tSubpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    tSubpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // Transition from initial to final
    tSubpassDependencies[1].srcSubpass = 0;
    tSubpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    tSubpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    tSubpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    tSubpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    tSubpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    tSubpassDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkSubpassDescription tSubpassDescription = {};
    tSubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    tSubpassDescription.flags = 0;
    tSubpassDescription.inputAttachmentCount = 0;
    tSubpassDescription.pInputAttachments = nullptr;
    tSubpassDescription.colorAttachmentCount = 1;
    tSubpassDescription.pColorAttachments = &tColorReference;
    tSubpassDescription.pResolveAttachments = nullptr;
    tSubpassDescription.pDepthStencilAttachment = nullptr;
    tSubpassDescription.preserveAttachmentCount = 0;
    tSubpassDescription.pPreserveAttachments = nullptr;

    VkRenderPassCreateInfo tRenderPassInfo = {};
    tRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    tRenderPassInfo.pNext = NULL;
    tRenderPassInfo.attachmentCount = 1;
    tRenderPassInfo.pAttachments = tAttachments;
    tRenderPassInfo.subpassCount = 1;
    tRenderPassInfo.pSubpasses = &tSubpassDescription;
    tRenderPassInfo.dependencyCount = 2;
    tRenderPassInfo.pDependencies = tSubpassDependencies;

    if (vkCreateRenderPass(ppLogicalDevice->GetDevice(), &tRenderPassInfo, nullptr, &poRenderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

