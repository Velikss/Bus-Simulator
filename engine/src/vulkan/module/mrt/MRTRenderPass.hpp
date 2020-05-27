#pragma once

#include <pch.hpp>
#include <vulkan/renderpass/RenderPass.hpp>
#include <vulkan/swapchain/SwapChain.hpp>

class cMRTRenderPass : public cRenderPass
{
private:
    cSwapChain* ppSwapChain;

public:
    cMRTRenderPass(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain);

protected:
    void CreateRenderPass();
};

cMRTRenderPass::cMRTRenderPass(cLogicalDevice* pLogicalDevice,
                               cSwapChain* pSwapChain) : cRenderPass(pLogicalDevice)
{
    assert(pSwapChain != nullptr);

    ppSwapChain = pSwapChain;
    CreateRenderPass();
}

void cMRTRenderPass::CreateRenderPass()
{
    std::array<VkAttachmentDescription, 5> atAttachmentDescriptions = {};
    for (uint i = 0; i < 5; i++)
    {
        atAttachmentDescriptions[i] = ppSwapChain->GetAttachment(i).tDescription;
    }

    std::vector<VkAttachmentReference> atColorReferences;
    atColorReferences.push_back({0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
    atColorReferences.push_back({1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
    atColorReferences.push_back({2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
    atColorReferences.push_back({4, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}); //-V112

    VkAttachmentReference depthReference = {};
    depthReference.attachment = 3;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // Use subpass dependencies for image layout transitions
    VkSubpassDependency tSubpassDependencies[2] = {};

    // Transition from final to initial (VK_SUBPASS_EXTERNAL refers to all commands executed outside of the actual renderpass)
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
    tSubpassDescription.pInputAttachments = NULL;
    tSubpassDescription.colorAttachmentCount = (uint) atColorReferences.size();
    tSubpassDescription.pColorAttachments = atColorReferences.data();
    tSubpassDescription.pResolveAttachments = NULL;
    tSubpassDescription.pDepthStencilAttachment = &depthReference;
    tSubpassDescription.preserveAttachmentCount = 0;
    tSubpassDescription.pPreserveAttachments = NULL;

    VkRenderPassCreateInfo tRenderPassInfo = {};
    tRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    tRenderPassInfo.pNext = NULL;
    tRenderPassInfo.attachmentCount = (uint) atAttachmentDescriptions.size();
    tRenderPassInfo.pAttachments = atAttachmentDescriptions.data();
    tRenderPassInfo.subpassCount = 1;
    tRenderPassInfo.pSubpasses = &tSubpassDescription;
    tRenderPassInfo.dependencyCount = 2;
    tRenderPassInfo.pDependencies = tSubpassDependencies;

    if (vkCreateRenderPass(ppLogicalDevice->GetDevice(), &tRenderPassInfo, nullptr, &poRenderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}
