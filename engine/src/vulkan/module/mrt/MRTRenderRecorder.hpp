#pragma once

#include <pch.hpp>
#include <vulkan/module/lighting/LightingRenderPass.hpp>
#include <vulkan/swapchain/SwapChain.hpp>
#include <vulkan/module/lighting/LightingPipeline.hpp>
#include "vulkan/command/CommandBufferRecorder.hpp"

class cMRTRenderRecorder : public iCommandBufferRecorder
{
private:
    cRenderPass* ppRenderPass;
    cSwapChain* ppSwapChain;
    cRenderPipeline* ppGraphicsPipeline;
    iUniformHandler* ppUniformHandler;
    cScene* ppScene;

    VkRenderPassBeginInfo ptRenderPassInfo = {};
    std::array<VkClearValue, 5> paoClearValues = {};

public:
    cMRTRenderRecorder(cRenderPass* pRenderPass,
                       cSwapChain* pSwapChain,
                       cRenderPipeline* pGraphicsPipeline,
                       iUniformHandler* pUniformHandler,
                       cScene* pScene);

    void Setup(uint uiIndex) override;
    void RecordCommands(VkCommandBuffer& oCommandBuffer, uint uiIndex) override;
};

cMRTRenderRecorder::cMRTRenderRecorder(cRenderPass* pRenderPass,
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

void cMRTRenderRecorder::Setup(uint uiIndex)
{
    // Struct with information about our render pass
    ptRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

    // Set the render pass and framebuffer
    ptRenderPassInfo.renderPass = ppRenderPass->GetRenderPass();
    ptRenderPassInfo.framebuffer = ppSwapChain->GetOffScreenFramebuffer();

    // Set the render area size
    ptRenderPassInfo.renderArea.offset = {0, 0};
    ptRenderPassInfo.renderArea.extent = ppSwapChain->ptSwapChainExtent;

    // Defines the clear color value to use
    paoClearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f}; // black with 100% opacity
    paoClearValues[1].color = {0.0f, 0.0f, 0.0f, 1.0f}; // black with 100% opacity
    paoClearValues[2].color = {0.0f, 0.0f, 0.0f, 1.0f}; // black with 100% opacity
    paoClearValues[3].depthStencil = {1.0f, 0}; // furthest possible depth
    paoClearValues[4].color = {0.0f, 0.0f, 0.0f, 1.0f}; // black with 100% opacity
    ptRenderPassInfo.clearValueCount = (uint) paoClearValues.size();
    ptRenderPassInfo.pClearValues = paoClearValues.data();
}

void cMRTRenderRecorder::RecordCommands(VkCommandBuffer& oCommandBuffer, uint uiIndex)
{
    // Begin the render pass
    vkCmdBeginRenderPass(oCommandBuffer, &ptRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Bind the graphics pipeline
    vkCmdBindPipeline(oCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      ppGraphicsPipeline->GetPipeline());

    uint uiGeometryIndex = 0;
    for (auto oObject : ppScene->GetObjects())
    {
        cGeometry* pGeometry = oObject.second->GetMesh()->GetGeometry();

        // Bind the vertex buffers
        pGeometry->CmdBindVertexBuffer(oCommandBuffer);

        // Bind the index buffers
        pGeometry->CmdBindIndexBuffer(oCommandBuffer);

        // Bind the descriptor sets
        ppUniformHandler->CmdBindDescriptorSets(oCommandBuffer,
                                                ppGraphicsPipeline->GetLayout(),
                                                uiGeometryIndex++);

        // Draw the vertices
        vkCmdDrawIndexed(oCommandBuffer, pGeometry->GetIndexCount(), 1, 0, 0, 0);
    }

    // End the render pass
    vkCmdEndRenderPass(oCommandBuffer);
}
