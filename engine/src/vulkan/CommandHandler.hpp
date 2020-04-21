#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/PhysicalDevice.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/SwapChain.hpp>
#include "RenderPass.hpp"
#include "GraphicsPipeline.hpp"
#include "VertexBuffer.hpp"

class cCommandHandler
{
private:
    cLogicalDevice* ppLogicalDevice;

    VkCommandPool poCommandPool;

    std::vector<VkCommandBuffer> paoCommandBuffers;

public:
    cCommandHandler(cPhysicalDevice* pPhysicalDevice,
                    cLogicalDevice* pLogicalDevice);
    ~cCommandHandler(void);

    void CreateCommandBuffers(cSwapChain* pSwapChain,
                              cRenderPass* pRenderPass,
                              cGraphicsPipeline* pGraphicsPipeline,
                              cVertexBuffer* pVertexBuffer);

    VkCommandBuffer& GetCommandBuffer(uint index);

private:
    void CreateCommandPool(cPhysicalDevice* pPhysicalDevice);
    void CreateCommandBuffers(cSwapChain* pSwapChain);
    void RecordCommandBuffers(cRenderPass* pRenderPass,
                              cSwapChain* pSwapChain,
                              cGraphicsPipeline* pGraphicsPipeline,
                              cVertexBuffer* pVertexBuffer);
};

cCommandHandler::cCommandHandler(cPhysicalDevice* pPhysicalDevice,
                                 cLogicalDevice* pLogicalDevice)
{
    ppLogicalDevice = pLogicalDevice;

    CreateCommandPool(pPhysicalDevice);
}

cCommandHandler::~cCommandHandler(void)
{
    ppLogicalDevice->DestroyCommandPool(poCommandPool, NULL);
}

void cCommandHandler::CreateCommandBuffers(cSwapChain* pSwapChain,
                                           cRenderPass* pRenderPass,
                                           cGraphicsPipeline* pGraphicsPipeline,
                                           cVertexBuffer* pVertexBuffer)
{
    CreateCommandBuffers(pSwapChain);
    RecordCommandBuffers(pRenderPass, pSwapChain, pGraphicsPipeline, pVertexBuffer);
}

void cCommandHandler::CreateCommandPool(cPhysicalDevice* pPhysicalDevice)
{
    // Find the supported queue families from the physical device
    tQueueFamilyIndices queueFamilyIndices = pPhysicalDevice->FindQueueFamilies();

    // Struct with information about the command pool
    VkCommandPoolCreateInfo tPoolInfo = {};
    tPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    tPoolInfo.queueFamilyIndex = queueFamilyIndices.oulGraphicsFamily.value();
    tPoolInfo.flags = 0;

    // Create the command pool
    if (!ppLogicalDevice->CreateCommandPool(&tPoolInfo, NULL, &poCommandPool))
    {
        throw std::runtime_error("failed to create command pool!");
    }
}

void cCommandHandler::CreateCommandBuffers(cSwapChain* pSwapChain)
{
    // Make the command buffer list the same size as the framebuffer list
    paoCommandBuffers.resize(pSwapChain->GetFramebufferSize());

    // Struct with information on how to allocate the command buffers
    VkCommandBufferAllocateInfo tAllocInfo = {};
    tAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

    // Set the command pool
    tAllocInfo.commandPool = poCommandPool;

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
                                           cVertexBuffer* pVertexBuffer)
{
    for (size_t i = 0; i < paoCommandBuffers.size(); i++)
    {
        // Struct with details about the usage of this buffer
        VkCommandBufferBeginInfo tBeginInfo = {};
        tBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // Optional flags for this command buffer
        tBeginInfo.flags = 0;

        // Used for secondary buffers, specifies which state to inherit
        tBeginInfo.pInheritanceInfo = NULL;

        // Begin recording the command buffer
        if (vkBeginCommandBuffer(paoCommandBuffers[i], &tBeginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        // We want to start with a BeginRenderPass command
        VkRenderPassBeginInfo tRenderPassInfo = {};
        tRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

        // Set the render pass and framebuffer
        tRenderPassInfo.renderPass = pRenderPass->poRenderPass;
        tRenderPassInfo.framebuffer = pSwapChain->GetFramebuffer(i);

        // Set the render area size
        tRenderPassInfo.renderArea.offset = {0, 0};
        tRenderPassInfo.renderArea.extent = pSwapChain->ptSwapChainExtent;

        // Defines the clear color value to use
        VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f}; // black with 100% opacity
        tRenderPassInfo.clearValueCount = 1;
        tRenderPassInfo.pClearValues = &clearColor;

        // Command buffer recording
        {
            // Begin the render pass
            vkCmdBeginRenderPass(paoCommandBuffers[i], &tRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            // Bind the graphics pipeline
            vkCmdBindPipeline(paoCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                              pGraphicsPipeline->poGraphicsPipeline);

            // Bind the vertex buffers
            pVertexBuffer->CmdBindVertexBuffer(paoCommandBuffers[i]);

            // Draw the vertices
            vkCmdDraw(paoCommandBuffers[i], pVertexBuffer->GetVertexCount(), 1, 0, 0);

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
