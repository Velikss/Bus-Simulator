#pragma once

#include <pch.hpp>
#include <vulkan/command/CommandBufferRecorder.hpp>
#include <vulkan/swapchain/SwapChain.hpp>
#include <vulkan/module/mrt/MRTUniformHandler.hpp>
#include "OverlayRenderPass.hpp"
#include "OverlayPipeline.hpp"

class cOverlayCommandBufferRecorder : public iCommandBufferRecorder
{
private:
    cRenderPass* ppRenderPass;
    cSwapChain* ppSwapChain;
    cRenderPipeline* ppPipeline;
    iUniformHandler* ppUniformHandler;

    VkRenderPassBeginInfo ptRenderPassInfo = {};
    std::array<VkClearValue, 1> paoClearValues = {};

    iGameManager* ppOverlayProvider;

public:
    cOverlayCommandBufferRecorder(cRenderPass* pRenderPass,
                                  cSwapChain* pSwapChain,
                                  cRenderPipeline* pGraphicsPipeline,
                                  iUniformHandler* pUniformHandler,
                                  iGameManager* pOverlayProvider);

    void Setup(uint uiIndex) override;
    void RecordCommands(VkCommandBuffer& oCommandBuffer, uint uiIndex) override;
};

cOverlayCommandBufferRecorder::cOverlayCommandBufferRecorder(cRenderPass* pRenderPass,
                                                             cSwapChain* pSwapChain,
                                                             cRenderPipeline* pGraphicsPipeline,
                                                             iUniformHandler* pUniformHandler,
                                                             iGameManager* pOverlayProvider)
{
    ppRenderPass = pRenderPass;
    ppSwapChain = pSwapChain;
    ppPipeline = pGraphicsPipeline;
    ppUniformHandler = pUniformHandler;
    ppOverlayProvider = pOverlayProvider;
}

void cOverlayCommandBufferRecorder::Setup(uint uiIndex)
{
    // Struct with information about our render pass
    ptRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

    // Set the render pass and framebuffer
    ptRenderPassInfo.renderPass = ppRenderPass->GetRenderPass();
    ptRenderPassInfo.framebuffer = ppSwapChain->GetOverlayFramebuffer();

    // Set the render area size
    ptRenderPassInfo.renderArea.offset = {0, 0};
    ptRenderPassInfo.renderArea.extent = ppSwapChain->ptSwapChainExtent;

    // Defines the clear color value to use
    paoClearValues[0].color = {0.0f, 0.0f, 0.0f, 0.0f}; // black with 0% opacity
    ptRenderPassInfo.clearValueCount = (uint) paoClearValues.size();
    ptRenderPassInfo.pClearValues = paoClearValues.data();
}

void cOverlayCommandBufferRecorder::RecordCommands(VkCommandBuffer& oCommandBuffer, uint uiIndex)
{
    // Begin the render pass
    vkCmdBeginRenderPass(oCommandBuffer, &ptRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Bind the graphics pipeline
    vkCmdBindPipeline(oCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      ppPipeline->GetPipeline());

    cOverlayWindow* pOverlayWindow = ppOverlayProvider->GetActiveOverlayWindow();
    if (pOverlayWindow != nullptr)
    {
        cUIManager* pUIManager = pOverlayWindow->GetUIManager();
        uint uiGeometryIndex = 0;
        for (auto& tElement : pUIManager->patElements)
        {
            for (uint uiIndex = 0; uiIndex < tElement.ppElement->GetChildCount(); uiIndex++)
            {
                pUIManager->CmdBindVertexBuffer(oCommandBuffer, &tElement, uiIndex);

                ppUniformHandler->CmdBindDescriptorSets(oCommandBuffer, ppPipeline->GetLayout(), uiGeometryIndex++);

                uint uiVertexCount = tElement.ppElement->GetVertexCount(uiIndex);
                vkCmdDraw(oCommandBuffer, uiVertexCount, 1, 0, 0);
            }
        }
    }

    // End the render pass
    vkCmdEndRenderPass(oCommandBuffer);
}
