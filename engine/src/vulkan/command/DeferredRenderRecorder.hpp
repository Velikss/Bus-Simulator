#pragma once

#include <pch.hpp>
#include <vulkan/GraphicsRenderPass.hpp>
#include <vulkan/SwapChain.hpp>
#include <vulkan/pipeline/GraphicsPipeline.hpp>
#include "CommandBufferRecorder.hpp"

class cLightingRenderRecorder : public iCommandBufferRecorder
{
private:
    cRenderPass* ppRenderPass;
    cSwapChain* ppSwapChain;
    cRenderPipeline* ppGraphicsPipeline;
    iUniformHandler* ppUniformHandler;
    cScene* ppScene;

    VkRenderPassBeginInfo ptRenderPassInfo = {};
    std::array<VkClearValue, 2> paoClearValues = {};

public:
    cLightingRenderRecorder(cRenderPass* pRenderPass,
                            cSwapChain* pSwapChain,
                            cRenderPipeline* pGraphicsPipeline,
                            iUniformHandler* pUniformHandler,
                            cScene* pScene);

    void Setup(uint uiIndex) override;
    void RecordCommands(VkCommandBuffer& oCommandBuffer, uint uiIndex) override;
};

cLightingRenderRecorder::cLightingRenderRecorder(cRenderPass* pRenderPass,
                                                 cSwapChain* pSwapChain,
                                                 cRenderPipeline* pGraphicsPipeline,
                                                 iUniformHandler* pUniformHandler,
                                                 cScene* pScene)
{
    ppRenderPass = pRenderPass;
    ppSwapChain = pSwapChain;
    ppGraphicsPipeline = pGraphicsPipeline;
    ppUniformHandler = pUniformHandler;
    ppScene = pScene;
}

void cLightingRenderRecorder::Setup(uint uiIndex)
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

void cLightingRenderRecorder::RecordCommands(VkCommandBuffer& oCommandBuffer, uint uiIndex)
{
    // Begin the render pass
    vkCmdBeginRenderPass(oCommandBuffer, &ptRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Bind the graphics pipeline
    vkCmdBindPipeline(oCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      ppGraphicsPipeline->GetPipeline());

    // Bind the descriptor sets
    ppUniformHandler->CmdBindDescriptorSets(oCommandBuffer,
                                            ppGraphicsPipeline->GetLayout(),
                                            0);

    cGeometry* pGeometry = ppScene->pQuadsGeometry;
    pGeometry->CmdBindVertexBuffer(oCommandBuffer);
    pGeometry->CmdBindIndexBuffer(oCommandBuffer);
    vkCmdDrawIndexed(oCommandBuffer, 6, 1, 0, 0, 1);

    // End the render pass
    vkCmdEndRenderPass(oCommandBuffer);
}
