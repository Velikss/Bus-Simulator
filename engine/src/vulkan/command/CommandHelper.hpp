#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/LogicalDevice.hpp>
#include <mutex>

// Helper class for using vulkan commands
class cCommandHelper
{
public:
    // TODO: This should be moved from a static field to a separate class
    static VkCommandPool poCommandPool;
    static std::mutex ptCommandPoolMutex;

    // Setup a command pool on a logical device
    static void SetupCommandPool(cLogicalDevice* pLogicalDevice);

    // Start a one time command and return the command buffer
    static VkCommandBuffer BeginSingleTimeCommands(cLogicalDevice* pLogicalDevice);
    // End and submit a one time command and clean up the command buffer
    static void EndSingleTimeCommands(cLogicalDevice* pLogicalDevice, VkCommandBuffer oCommandBuffer);
};

VkCommandPool cCommandHelper::poCommandPool = VK_NULL_HANDLE;
std::mutex cCommandHelper::ptCommandPoolMutex;

void cCommandHelper::SetupCommandPool(cLogicalDevice* pLogicalDevice)
{
    assert(pLogicalDevice != nullptr);

    // Find the supported queue families from the physical device
    tQueueFamilyIndices queueFamilyIndices = cPhysicalDevice::GetInstance()->FindQueueFamilies();

    // Struct with information about the command pool
    VkCommandPoolCreateInfo tPoolInfo = {};
    tPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    tPoolInfo.queueFamilyIndex = queueFamilyIndices.oulGraphicsFamily.value();
    tPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    // Create the command pool
    if (!pLogicalDevice->CreateCommandPool(&tPoolInfo, nullptr, &poCommandPool))
    {
        throw std::runtime_error("failed to create command pool!");
    }
}

VkCommandBuffer cCommandHelper::BeginSingleTimeCommands(cLogicalDevice* pLogicalDevice)
{
    assert(pLogicalDevice != nullptr);          // logical device must exist
    assert(poCommandPool != VK_NULL_HANDLE);    // command pool has to be set up first

    ptCommandPoolMutex.lock();

    // Struct with information about how the command buffer should be allocated
    VkCommandBufferAllocateInfo tAllocInfo = {};
    tAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    tAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    tAllocInfo.commandPool = poCommandPool;
    tAllocInfo.commandBufferCount = 1;

    // Allocate the command buffer
    VkCommandBuffer oCommandBuffer;
    pLogicalDevice->AllocateCommandBuffers(&tAllocInfo, &oCommandBuffer);

    // Struct with information for beginning the command buffer recording
    VkCommandBufferBeginInfo tBeginInfo = {};
    tBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    tBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    // Begin the command buffer recording
    vkBeginCommandBuffer(oCommandBuffer, &tBeginInfo);

    // Return the command buffer
    return oCommandBuffer;
}

void cCommandHelper::EndSingleTimeCommands(cLogicalDevice* pLogicalDevice, VkCommandBuffer oCommandBuffer)
{
    assert(pLogicalDevice != nullptr);          // logical device must exist
    assert(oCommandBuffer != VK_NULL_HANDLE);   // command buffer must exist

    // End the command buffer recording
    vkEndCommandBuffer(oCommandBuffer);

    // Struct with information about the queue submit we want to do
    VkSubmitInfo tSubmitInfo = {};
    tSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    tSubmitInfo.commandBufferCount = 1;
    tSubmitInfo.pCommandBuffers = &oCommandBuffer;

    // Submit the command buffer to the queue and wait until it's idle again
    VkFence oFence = VK_NULL_HANDLE;
    pLogicalDevice->GraphicsQueueSubmit(1, &tSubmitInfo, oFence);
    pLogicalDevice->GraphicsQueueWaitIdle();

    // Free the command buffer
    pLogicalDevice->FreeCommandBuffers(poCommandPool, 1, &oCommandBuffer);

    ptCommandPoolMutex.unlock();
}
