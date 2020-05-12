#pragma once

#include <pch.hpp>
#include <vulkan/module/lighting/LightingRenderPass.hpp>
#include <vulkan/swapchain/SwapChain.hpp>
#include <vulkan/command/CommandBufferRecorder.hpp>

class cClearScreenRecorder : public iCommandBufferRecorder
{
private:
    cRenderPass* ppRenderPass;
    cSwapChain* ppSwapChain;

    VkRenderPassBeginInfo ptRenderPassInfo = {};
    std::array<VkClearValue, 2> paoClearValues = {};

public:
    cClearScreenRecorder(cRenderPass* pRenderPass,
                         cSwapChain* pSwapChain);

    void Setup(uint uiIndex) override;
    void RecordCommands(VkCommandBuffer& oCommandBuffer, uint uiIndex) override;
};

cClearScreenRecorder::cClearScreenRecorder(cRenderPass* pRenderPass,
                                           cSwapChain* pSwapChain)
{
    ppRenderPass = pRenderPass;
    ppSwapChain = pSwapChain;
}

void cClearScreenRecorder::Setup(uint uiIndex)
{
    // Struct with information about our render pass
    ptRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

    // Set the render pass and framebuffer
    ptRenderPassInfo.renderPass = ppRenderPass->GetRenderPass();
    ptRenderPassInfo.framebuffer = ppSwapChain->GetFramebuffer(uiIndex);

    // Set the render area size
    ptRenderPassInfo.renderArea.offset = {0, 0};
    ptRenderPassInfo.renderArea.extent = ppSwapChain->ptSwapChainExtent;

    // Defines the clear color value to use
    paoClearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f}; // black with 100% opacity
    paoClearValues[1].depthStencil = {1.0f, 0}; // furthest possible depth
    ptRenderPassInfo.clearValueCount = paoClearValues.size();
    ptRenderPassInfo.pClearValues = paoClearValues.data();
}

void cClearScreenRecorder::RecordCommands(VkCommandBuffer& oCommandBuffer, uint uiIndex)
{
    // Begin the render pass
    vkCmdBeginRenderPass(oCommandBuffer, &ptRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // End the render pass
    vkCmdEndRenderPass(oCommandBuffer);
}
