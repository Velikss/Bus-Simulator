#pragma once

#include <pch.hpp>
#include <vulkan/RenderPass.hpp>
#include <vulkan/SwapChain.hpp>
#include <vulkan/GraphicsPipeline.hpp>
#include "CommandBufferRecorder.hpp"

class cIndexedRenderRecorder : public cCommandBufferRecorder
{
private:
    cRenderPass* ppRenderPass;
    cSwapChain* ppSwapChain;
    cGraphicsPipeline* ppGraphicsPipeline;
    std::vector<cMesh*>* ppMeshes;
    cUniformHandler* ppUniformHandler;

    VkRenderPassBeginInfo ptRenderPassInfo = {};
    std::array<VkClearValue, 2> paoClearValues = {};

public:
    cIndexedRenderRecorder(cRenderPass* pRenderPass,
                           cSwapChain* pSwapChain,
                           cGraphicsPipeline* pGraphicsPipeline,
                           std::vector<cMesh*>* paMeshes,
                           cUniformHandler* pUniformHandler);

    void Setup(uint uiIndex) override;
    void RecordCommands(VkCommandBuffer& oCommandBuffer, uint uiIndex) override;
};

cIndexedRenderRecorder::cIndexedRenderRecorder(cRenderPass* pRenderPass,
                                               cSwapChain* pSwapChain,
                                               cGraphicsPipeline* pGraphicsPipeline,
                                               std::vector<cMesh*>* paMeshes,
                                               cUniformHandler* pUniformHandler)
{
    ppRenderPass = pRenderPass;
    ppSwapChain = pSwapChain;
    ppGraphicsPipeline = pGraphicsPipeline;
    ppMeshes = paMeshes;
    ppUniformHandler = pUniformHandler;
}

void cIndexedRenderRecorder::Setup(uint uiIndex)
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
    paoClearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f}; // black with 100% opacity
    paoClearValues[1].depthStencil = {1.0f, 0}; // furthest possible depth
    ptRenderPassInfo.clearValueCount = paoClearValues.size();
    ptRenderPassInfo.pClearValues = paoClearValues.data();
}

void cIndexedRenderRecorder::RecordCommands(VkCommandBuffer& oCommandBuffer, uint uiIndex)
{
    // Begin the render pass
    vkCmdBeginRenderPass(oCommandBuffer, &ptRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Bind the graphics pipeline
    vkCmdBindPipeline(oCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      ppGraphicsPipeline->poGraphicsPipeline);

    for (uint uiGeometryIndex = 0; uiGeometryIndex < ppMeshes->size(); uiGeometryIndex++)
    {
        cGeometry* pGeometry = (*ppMeshes)[uiGeometryIndex]->GetGeometry();

        // Bind the vertex buffers
        pGeometry->CmdBindVertexBuffer(oCommandBuffer);

        // Bind the index buffers
        pGeometry->CmdBindIndexBuffer(oCommandBuffer);

        // Bind the descriptor sets
        ppUniformHandler->CmdBindDescriptorSets(oCommandBuffer,
                                                ppGraphicsPipeline->poPipelineLayout,
                                                uiGeometryIndex);

        // Draw the vertices
        vkCmdDrawIndexed(oCommandBuffer, pGeometry->GetIndexCount(), 1, 0, 0, 0);
    }

    // End the render pass
    vkCmdEndRenderPass(oCommandBuffer);
}
