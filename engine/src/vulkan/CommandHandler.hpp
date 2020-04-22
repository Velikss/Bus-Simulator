#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/PhysicalDevice.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/SwapChain.hpp>
#include <vulkan/RenderPass.hpp>
#include <vulkan/GraphicsPipeline.hpp>
#include <vulkan/VertexBuffer.hpp>
#include "CommandHelper.hpp"

class cCommandHandler
{
private:
    cLogicalDevice* ppLogicalDevice;

    std::vector<VkCommandBuffer> paoCommandBuffers;

public:
    cCommandHandler(cLogicalDevice* pLogicalDevice);
    ~cCommandHandler(void);

    void CreateCommandBuffers(cSwapChain* pSwapChain,
                              cRenderPass* pRenderPass,
                              cGraphicsPipeline* pGraphicsPipeline,
                              cVertexBuffer* pVertexBuffer,
                              cUniformHandler* pUniformHandler);

    VkCommandBuffer& GetCommandBuffer(uint index);

private:
    void CreateCommandBuffers(cSwapChain* pSwapChain);
    void RecordCommandBuffers(cRenderPass* pRenderPass,
                              cSwapChain* pSwapChain,
                              cGraphicsPipeline* pGraphicsPipeline,
                              cVertexBuffer* pVertexBuffer,
                              cUniformHandler* pUniformHandler);
};

cCommandHandler::cCommandHandler(cLogicalDevice* pLogicalDevice)
{
    ppLogicalDevice = pLogicalDevice;

    cCommandHelper::SetupCommandPool(pLogicalDevice);
}

cCommandHandler::~cCommandHandler(void)
{
    ppLogicalDevice->FreeCommandBuffers(cCommandHelper::poCommandPool,
                                        paoCommandBuffers.size(),
                                        paoCommandBuffers.data());
    ppLogicalDevice->DestroyCommandPool(cCommandHelper::poCommandPool, nullptr);
}

void cCommandHandler::CreateCommandBuffers(cSwapChain* pSwapChain,
                                           cRenderPass* pRenderPass,
                                           cGraphicsPipeline* pGraphicsPipeline,
                                           cVertexBuffer* pVertexBuffer,
                                           cUniformHandler* pUniformHandler)
{
    CreateCommandBuffers(pSwapChain);
    RecordCommandBuffers(pRenderPass, pSwapChain, pGraphicsPipeline, pVertexBuffer, pUniformHandler);
}

void cCommandHandler::CreateCommandBuffers(cSwapChain* pSwapChain)
{
    // Make the command buffer list the same size as the framebuffer list
    paoCommandBuffers.resize(pSwapChain->GetFramebufferSize());

    // Struct with information on how to allocate the command buffers
    VkCommandBufferAllocateInfo tAllocInfo = {};
    tAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

    // Set the command pool
    tAllocInfo.commandPool = cCommandHelper::poCommandPool;

    // Specify if the allocated buffers should be primary or secondary command buffers
    tAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    // Number of command buffers to allocate
    tAllocInfo.commandBufferCount = (uint) paoCommandBuffers.size();

    // Allocate the command buffers
    if (!ppLogicalDevice->AllocateCommandBuffers(&tAllocInfo, paoCommandBuffers.data()))
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void cCommandHandler::RecordCommandBuffers(cRenderPass* pRenderPass,
                                           cSwapChain* pSwapChain,
                                           cGraphicsPipeline* pGraphicsPipeline,
                                           cVertexBuffer* pVertexBuffer,
                                           cUniformHandler* pUniformHandler)
{
    for (size_t i = 0; i < paoCommandBuffers.size(); i++)
    {
        // Struct with details about the usage of this buffer
        VkCommandBufferBeginInfo tBeginInfo = {};
        tBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // Optional flags for this command buffer
        tBeginInfo.flags = 0;

        // Used for secondary buffers, specifies which state to inherit
        tBeginInfo.pInheritanceInfo = nullptr;

        // Struct with information about our render pass
        VkRenderPassBeginInfo tRenderPassInfo = {};
        tRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

        // Set the render pass and framebuffer
        tRenderPassInfo.renderPass = pRenderPass->poRenderPass;
        tRenderPassInfo.framebuffer = pSwapChain->GetFramebuffer(i);

        // Set the render area size
        tRenderPassInfo.renderArea.offset = {0, 0};
        tRenderPassInfo.renderArea.extent = pSwapChain->ptSwapChainExtent;

        // Defines the clear color value to use
        std::array<VkClearValue, 2> aoClearValues = {};
        aoClearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f}; // black with 100% opacity
        aoClearValues[1].depthStencil = {1.0f, 0}; // furthest possible depth
        tRenderPassInfo.clearValueCount = aoClearValues.size();
        tRenderPassInfo.pClearValues = aoClearValues.data();

        // Begin recording the command buffer
        if (vkBeginCommandBuffer(paoCommandBuffers[i], &tBeginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        // Command buffer recording
        {
            // Begin the render pass
            vkCmdBeginRenderPass(paoCommandBuffers[i], &tRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            // Bind the graphics pipeline
            vkCmdBindPipeline(paoCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                              pGraphicsPipeline->poGraphicsPipeline);

            // Bind the vertex buffers
            pVertexBuffer->CmdBindVertexBuffer(paoCommandBuffers[i]);

            // Bind the index buffers
            pVertexBuffer->CmdBindIndexBuffer(paoCommandBuffers[i]);

            pUniformHandler->CmdBindDescriptorSets(paoCommandBuffers[i], pGraphicsPipeline->poPipelineLayout, i);

            // Draw the vertices
            vkCmdDrawIndexed(paoCommandBuffers[i], pVertexBuffer->GetIndexCount(), 1, 0, 0, 0);

            // End the render pass
            vkCmdEndRenderPass(paoCommandBuffers[i]);
        }

        // End recording to the command buffer
        if (vkEndCommandBuffer(paoCommandBuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
}

VkCommandBuffer& cCommandHandler::GetCommandBuffer(uint index)
{
    return paoCommandBuffers[index];
}
