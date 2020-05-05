#pragma once

#include <pch.hpp>
#include <vulkan/command/CommandBufferRecorder.hpp>
#include <vulkan/SwapChain.hpp>
#include <vulkan/uniform/GraphicsUniformHandler.hpp>
#include "OverlayRenderPass.hpp"
#include "OverlayPipeline.hpp"
#include "Text.hpp"

class cOverlayCommandBufferRecorder : public iCommandBufferRecorder
{
private:
    cRenderPass* ppRenderPass;
    cSwapChain* ppSwapChain;
    cOverlayPipeline* ppGraphicsPipeline;
    iUniformHandler* ppUniformHandler;

    VkRenderPassBeginInfo ptRenderPassInfo = {};
    std::array<VkClearValue, 2> paoClearValues = {};

    cText* ppText;

public:
    cOverlayCommandBufferRecorder(cRenderPass* pRenderPass,
                                  cSwapChain* pSwapChain,
                                  cOverlayPipeline* pGraphicsPipeline,
                                  iUniformHandler* pUniformHandler,
                                  cText* pText);

    void Setup(uint uiIndex) override;
    void RecordCommands(VkCommandBuffer& oCommandBuffer, uint uiIndex) override;
};

cOverlayCommandBufferRecorder::cOverlayCommandBufferRecorder(cRenderPass* pRenderPass,
                                                             cSwapChain* pSwapChain,
                                                             cOverlayPipeline* pGraphicsPipeline,
                                                             iUniformHandler* pUniformHandler,
                                                             cText* pText)
{
    ppRenderPass = pRenderPass;
    ppSwapChain = pSwapChain;
    ppGraphicsPipeline = pGraphicsPipeline;
    ppUniformHandler = pUniformHandler;
    ppText = pText;
}

void cOverlayCommandBufferRecorder::Setup(uint uiIndex)
{
    // Struct with information about our render pass
    ptRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

    // Set the render pass and framebuffer
    ptRenderPassInfo.renderPass = ppRenderPass->poRenderPass;
    ptRenderPassInfo.framebuffer = ppSwapChain->GetFramebuffer(uiIndex);

    // Set the render area size
    ptRenderPassInfo.renderArea.offset = {0, 0};
    ptRenderPassInfo.renderArea.extent = ppSwapChain->ptSwapChainExtent;

    // Defines the clear color value to use
    paoClearValues[0].color = {0.0f, 0.0f, 0.0f, 0.0f}; // black with 100% opacity
    paoClearValues[1].depthStencil = {1.0f, 0}; // furthest possible depth
    ptRenderPassInfo.clearValueCount = paoClearValues.size();
    ptRenderPassInfo.pClearValues = paoClearValues.data();
}

void cOverlayCommandBufferRecorder::RecordCommands(VkCommandBuffer& oCommandBuffer, uint uiIndex)
{
    // Begin the render pass
    vkCmdBeginRenderPass(oCommandBuffer, &ptRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Bind the graphics pipeline
    vkCmdBindPipeline(oCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      ppGraphicsPipeline->poGraphicsPipeline);

    ppUniformHandler->CmdBindDescriptorSets(oCommandBuffer, ppGraphicsPipeline->poPipelineLayout, 0);

    ppText->BindVertexBuffer(oCommandBuffer);

    for (uint i = 0; i < ppText->GetNumLetters(); i++)
    {
        vkCmdDraw(oCommandBuffer, 4, 1, i * 4, 0);
    }

    // End the render pass
    vkCmdEndRenderPass(oCommandBuffer);
}
