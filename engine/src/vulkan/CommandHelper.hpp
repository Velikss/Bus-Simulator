#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/LogicalDevice.hpp>

class cCommandHelper
{
public:
    static VkCommandPool poCommandPool;

    static void SetupCommandPool(cLogicalDevice* pLogicalDevice);

    static VkCommandBuffer BeginSingleTimeCommands(cLogicalDevice* pLogicalDevice);
    static void EndSingleTimeCommands(cLogicalDevice* pLogicalDevice, VkCommandBuffer oCommandBuffer);
};

VkCommandPool cCommandHelper::poCommandPool = VK_NULL_HANDLE;

void cCommandHelper::SetupCommandPool(cLogicalDevice* pLogicalDevice)
{
    assert(pLogicalDevice != nullptr);

    // Find the supported queue families from the physical device
    tQueueFamilyIndices queueFamilyIndices = cPhysicalDevice::GetInstance()->FindQueueFamilies();

    // Struct with information about the command pool
    VkCommandPoolCreateInfo tPoolInfo = {};
    tPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    tPoolInfo.queueFamilyIndex = queueFamilyIndices.oulGraphicsFamily.value();
    tPoolInfo.flags = 0;

    // Create the command pool
    if (!pLogicalDevice->CreateCommandPool(&tPoolInfo, nullptr, &poCommandPool))
    {
        throw std::runtime_error("failed to create command pool!");
    }
}

VkCommandBuffer cCommandHelper::BeginSingleTimeCommands(cLogicalDevice* pLogicalDevice)
{
    assert(pLogicalDevice != nullptr);
    assert(poCommandPool != VK_NULL_HANDLE); // command pool has to be set up first

    VkCommandBufferAllocateInfo tAllocInfo = {};
    tAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    tAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    tAllocInfo.commandPool = poCommandPool;
    tAllocInfo.commandBufferCount = 1;

    VkCommandBuffer oCommandBuffer;
    pLogicalDevice->AllocateCommandBuffers(&tAllocInfo, &oCommandBuffer);

    VkCommandBufferBeginInfo tBeginInfo = {};
    tBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    tBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(oCommandBuffer, &tBeginInfo);

    return oCommandBuffer;
}

void cCommandHelper::EndSingleTimeCommands(cLogicalDevice* pLogicalDevice, VkCommandBuffer oCommandBuffer)
{
    assert(pLogicalDevice != nullptr);

    vkEndCommandBuffer(oCommandBuffer);

    VkSubmitInfo tSubmitInfo = {};
    tSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    tSubmitInfo.commandBufferCount = 1;
    tSubmitInfo.pCommandBuffers = &oCommandBuffer;

    VkFence oFence = VK_NULL_HANDLE;
    pLogicalDevice->GraphicsQueueSubmit(1, &tSubmitInfo, oFence);
    pLogicalDevice->GraphicsQueueWaitIdle();

    pLogicalDevice->FreeCommandBuffers(poCommandPool, 1, &oCommandBuffer);
}
