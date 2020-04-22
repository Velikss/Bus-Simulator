#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/PhysicalDevice.hpp>

class cLogicalDevice
{
private:
    // Logical device handle
    VkDevice poDevice;

    // Supported QueueFamilies
    tQueueFamilyIndices ptQueueIndices;

    // Handle for interfacing with the graphics queue
    VkQueue poGraphicsQueue;

    // Handle for interfacing with the presentation queue
    VkQueue poPresentQueue;

public:
    cLogicalDevice(void);
    ~cLogicalDevice(void);

    VkDevice& GetDevice(void);

    void WaitUntilIdle(void);

    bool AllocateCommandBuffers(VkCommandBufferAllocateInfo* pAllocateInfo,
                                VkCommandBuffer* pCommandBuffers);
    void FreeCommandBuffers(VkCommandPool& oCommandPool,
                            uint uiCommandBufferCount,
                            VkCommandBuffer* pCommandBuffers);

    bool CreateCommandPool(VkCommandPoolCreateInfo* pCreateInfo,
                           VkAllocationCallbacks* pAllocator,
                           VkCommandPool* pCommandPool);
    void DestroyCommandPool(VkCommandPool& oCommandPool,
                            VkAllocationCallbacks* pAllocator);

    bool CreateShaderModule(VkShaderModuleCreateInfo* pCreateInfo,
                            VkAllocationCallbacks* pAllocator,
                            VkShaderModule* pShaderModule);
    void DestroyShaderModule(VkShaderModule& oShaderModule,
                             VkAllocationCallbacks* pAllocator);

    bool CreatePipelineLayout(VkPipelineLayoutCreateInfo* pCreateInfo,
                              VkAllocationCallbacks* pAllocator,
                              VkPipelineLayout* pPipelineLayout);
    void DestroyPipelineLayout(VkPipelineLayout& oPipelineLayout,
                               VkAllocationCallbacks* pAllocator);

    bool CreateGraphicsPipeline(uint uiCreateInfoCount,
                                VkGraphicsPipelineCreateInfo* pCreateInfos,
                                VkPipeline* pPipelines);
    void DestroyPipeline(VkPipeline& oPipeline,
                         VkAllocationCallbacks* pAllocator);

    bool CreateBuffer(VkBufferCreateInfo* pCreateInfo,
                      VkAllocationCallbacks* pAllocator,
                      VkBuffer* pBuffer);
    void DestroyBuffer(VkBuffer& oBuffer,
                       VkAllocationCallbacks* pAllocator);

    bool AllocateMemory(VkMemoryAllocateInfo* pAllocateInfo,
                        VkAllocationCallbacks* pAllocator,
                        VkDeviceMemory* pMemory);
    void FreeMemory(VkDeviceMemory& oMemory,
                    VkAllocationCallbacks* pAllocator);
    void BindBufferMemory(VkBuffer& oBuffer,
                          VkDeviceMemory& oMemory,
                          VkDeviceSize ulMemoryOffset);

    void MapMemory(VkDeviceMemory& oMemory,
                   VkDeviceSize ulOffset,
                   VkDeviceSize ulSize,
                   VkMemoryMapFlags uiFlags,
                   void** ppData);
    void UnmapMemory(VkDeviceMemory& oDeviceMemory);

    bool GraphicsQueueSubmit(uint uiSubmitCount,
                             VkSubmitInfo* ptSubmitInfo,
                             VkFence& oFence);
    void GraphicsQueueWaitIdle(void);

    void QueuePresent(VkPresentInfoKHR* ptPresentInfo);

    void WaitForFences(uint uiFenceCount,
                       VkFence* pFences,
                       VkBool32 bWaitAll,
                       uint64 ulTimeout);
    void ResetFences(uint uiFenceCount,
                     VkFence* pFences);

    bool CreateDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo* pCreateInfo,
                                   VkAllocationCallbacks* pAllocator,
                                   VkDescriptorSetLayout* pSetLayout);
    void DestroyDescriptorSetLayout(VkDescriptorSetLayout& oDescriptorSetLayout,
                                    VkAllocationCallbacks* pAllocator);

    bool CreateDescriptorPool(VkDescriptorPoolCreateInfo* pCreateInfo,
                              VkAllocationCallbacks* pAllocator,
                              VkDescriptorPool* pDescriptorPool);
    void DestroyDescriptorPool(VkDescriptorPool& oDescriptorPool,
                               VkAllocationCallbacks* pAllocator);

    bool AllocateDescriptorSets(VkDescriptorSetAllocateInfo* pAllocateInfo,
                                VkDescriptorSet* pDescriptorSets);
    void FreeDescriptorSets(VkDescriptorPool& oDescriptorPool,
                            uint uiDescriptorSetCount,
                            VkDescriptorSet* pDescriptorSets);
    void UpdateDescriptorSets(uint uiDescriptorWriteCount,
                              VkWriteDescriptorSet* pDescriptorWrites,
                              uint uiDescriptorCopyCount,
                              VkCopyDescriptorSet* pDescriptorCopies);

    bool CreateImage(VkImageCreateInfo* pCreateInfo,
                     VkAllocationCallbacks* pAllocator,
                     VkImage* pImage);
    void GetImageMemoryRequirements(VkImage& oImage,
                                    VkMemoryRequirements* pMemoryRequirements);
    void BindImageMemory(VkImage& oImage,
                         VkDeviceMemory& oMemory,
                         VkDeviceSize ulMemoryOffset);

    bool CreateImageView(VkImageViewCreateInfo* pCreateInfo,
                         VkAllocationCallbacks* pAllocator,
                         VkImageView* pView);


private:
    VkDeviceQueueCreateInfo GetQueueCreateInfo(uint uiQueueFamily);
    VkDeviceCreateInfo GetDeviceCreateInfo(std::vector<VkDeviceQueueCreateInfo>& atQueueCreateInfos,
                                           VkPhysicalDeviceFeatures& tDeviceFeatures,
                                           const std::vector<const char*>& apDeviceExtensions);
};

cLogicalDevice::cLogicalDevice()
{
    cPhysicalDevice* pPhysicalDevice = cPhysicalDevice::GetInstance();

    // Get supported QueueFamilies from the physical device
    ptQueueIndices = pPhysicalDevice->FindQueueFamilies();

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    uint uniqueQueueFamilies[] = {
            ptQueueIndices.oulGraphicsFamily.value(),
            ptQueueIndices.oulPresentFamily.value()
    };

    // Struct with information about the physical device features we want to enable
    VkPhysicalDeviceFeatures tDeviceFeatures = {};

    for (uint uiQueueFamily : uniqueQueueFamilies)
    {
        queueCreateInfos.push_back(GetQueueCreateInfo(uiQueueFamily));
    }

    VkDeviceCreateInfo tDeviceCreateInfo = GetDeviceCreateInfo(queueCreateInfos, tDeviceFeatures,
                                                               pPhysicalDevice->DEVICE_EXTENSIONS);

    // Create the logical device, and throw an error on failure
    if (!pPhysicalDevice->CreateLogicalDevice(&tDeviceCreateInfo, nullptr, &poDevice))
    {
        throw std::runtime_error("failed to create logical device!");
    }

    // Get the graphics queue handle from the logical device
    vkGetDeviceQueue(poDevice, ptQueueIndices.oulGraphicsFamily.value(), 0, &poGraphicsQueue);

    // Get the presentation queue handle from the logical device
    vkGetDeviceQueue(poDevice, ptQueueIndices.oulPresentFamily.value(), 0, &poPresentQueue);
}

cLogicalDevice::~cLogicalDevice()
{
    // Destroy the logical device
    vkDestroyDevice(poDevice, nullptr);
}

VkDeviceQueueCreateInfo cLogicalDevice::GetQueueCreateInfo(uint uiQueueFamily)
{
    // Struct with information about for creating the Queue
    VkDeviceQueueCreateInfo tQueueCreateInfo = {};
    tQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    tQueueCreateInfo.queueCount = 1;

    // Set the family index
    tQueueCreateInfo.queueFamilyIndex = uiQueueFamily;

    // Priority is required even with only a single queue
    float fQueuePriority = 1.0f;
    tQueueCreateInfo.pQueuePriorities = &fQueuePriority;

    return tQueueCreateInfo;
}

VkDeviceCreateInfo cLogicalDevice::GetDeviceCreateInfo(std::vector<VkDeviceQueueCreateInfo>& atQueueCreateInfos,
                                                       VkPhysicalDeviceFeatures& tDeviceFeatures,
                                                       const std::vector<const char*>& apDeviceExtensions)
{
    // Struct with info for creating the logical device
    VkDeviceCreateInfo tCreateInfo = {};
    tCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    // Set the queue create info
    tCreateInfo.queueCreateInfoCount = static_cast<uint>(atQueueCreateInfos.size());
    tCreateInfo.pQueueCreateInfos = atQueueCreateInfos.data();

    // Set the device features we want to enable
    tCreateInfo.pEnabledFeatures = &tDeviceFeatures;

    // Set the device extensions we want to enable
    tCreateInfo.enabledExtensionCount = static_cast<uint>(apDeviceExtensions.size());
    tCreateInfo.ppEnabledExtensionNames = apDeviceExtensions.data();

    // Setup validation layers if they are enabled
    tCreateInfo.enabledLayerCount = 0;

    return tCreateInfo;
}

VkDevice& cLogicalDevice::GetDevice()
{
    return poDevice;
}

bool cLogicalDevice::GraphicsQueueSubmit(uint uiSubmitCount, VkSubmitInfo* ptSubmitInfo, VkFence& oFence)
{
    return vkQueueSubmit(poGraphicsQueue, uiSubmitCount, ptSubmitInfo, oFence) == VK_SUCCESS;
}

void cLogicalDevice::GraphicsQueueWaitIdle(void)
{
    vkQueueWaitIdle(poGraphicsQueue);
}

void cLogicalDevice::QueuePresent(VkPresentInfoKHR* ptPresentInfo)
{
    vkQueuePresentKHR(poPresentQueue, ptPresentInfo);
}

void cLogicalDevice::WaitForFences(uint uiFenceCount, VkFence* pFences, VkBool32 bWaitAll, uint64 ulTimeout)
{
    vkWaitForFences(poDevice, uiFenceCount, pFences, bWaitAll, ulTimeout);
}

void cLogicalDevice::ResetFences(uint uiFenceCount, VkFence* pFences)
{
    vkResetFences(poDevice, uiFenceCount, pFences);
}

void cLogicalDevice::WaitUntilIdle(void)
{
    vkDeviceWaitIdle(poDevice);
}

bool cLogicalDevice::AllocateCommandBuffers(VkCommandBufferAllocateInfo* pAllocateInfo,
                                            VkCommandBuffer* pCommandBuffers)
{
    return vkAllocateCommandBuffers(poDevice, pAllocateInfo, pCommandBuffers) == VK_SUCCESS;
}

bool cLogicalDevice::CreateCommandPool(VkCommandPoolCreateInfo* pCreateInfo,
                                       VkAllocationCallbacks* pAllocator,
                                       VkCommandPool* pCommandPool)
{
    return vkCreateCommandPool(poDevice, pCreateInfo, pAllocator, pCommandPool) == VK_SUCCESS;
}

void cLogicalDevice::DestroyCommandPool(VkCommandPool& oCommandPool, VkAllocationCallbacks* pAllocator)
{
    vkDestroyCommandPool(poDevice, oCommandPool, pAllocator);
}

bool cLogicalDevice::CreateShaderModule(VkShaderModuleCreateInfo* pCreateInfo,
                                        VkAllocationCallbacks* pAllocator,
                                        VkShaderModule* pShaderModule)
{
    return vkCreateShaderModule(poDevice, pCreateInfo, pAllocator, pShaderModule) == VK_SUCCESS;
}

void cLogicalDevice::DestroyShaderModule(VkShaderModule& oShaderModule,
                                         VkAllocationCallbacks* pAllocator)
{
    vkDestroyShaderModule(poDevice, oShaderModule, pAllocator);
}

bool cLogicalDevice::CreatePipelineLayout(VkPipelineLayoutCreateInfo* pCreateInfo,
                                          VkAllocationCallbacks* pAllocator,
                                          VkPipelineLayout* pPipelineLayout)
{
    return vkCreatePipelineLayout(poDevice, pCreateInfo, pAllocator, pPipelineLayout) == VK_SUCCESS;
}

void cLogicalDevice::DestroyPipelineLayout(VkPipelineLayout& oPipelineLayout, VkAllocationCallbacks* pAllocator)
{
    vkDestroyPipelineLayout(poDevice, oPipelineLayout, pAllocator);
}

bool cLogicalDevice::CreateGraphicsPipeline(uint uiCreateInfoCount,
                                            VkGraphicsPipelineCreateInfo* pCreateInfos,
                                            VkPipeline* pPipelines)
{
    return vkCreateGraphicsPipelines(poDevice, VK_NULL_HANDLE, uiCreateInfoCount, pCreateInfos, nullptr, pPipelines)
           == VK_SUCCESS;
}

void cLogicalDevice::DestroyPipeline(VkPipeline& oPipeline, VkAllocationCallbacks* pAllocator)
{
    vkDestroyPipeline(poDevice, oPipeline, pAllocator);
}

bool cLogicalDevice::CreateBuffer(VkBufferCreateInfo* pCreateInfo, VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer)
{
    return vkCreateBuffer(poDevice, pCreateInfo, pAllocator, pBuffer) == VK_SUCCESS;
}

void cLogicalDevice::DestroyBuffer(VkBuffer& oBuffer, VkAllocationCallbacks* pAllocator)
{
    vkDestroyBuffer(poDevice, oBuffer, pAllocator);
}

bool cLogicalDevice::AllocateMemory(VkMemoryAllocateInfo* pAllocateInfo,
                                    VkAllocationCallbacks* pAllocator,
                                    VkDeviceMemory* pMemory)
{
    return vkAllocateMemory(poDevice, pAllocateInfo, pAllocator, pMemory) == VK_SUCCESS;
}

void cLogicalDevice::FreeMemory(VkDeviceMemory& oMemory, VkAllocationCallbacks* pAllocator)
{
    vkFreeMemory(poDevice, oMemory, pAllocator);
}

void cLogicalDevice::BindBufferMemory(VkBuffer& oBuffer, VkDeviceMemory& oMemory, VkDeviceSize ulMemoryOffset)
{
    vkBindBufferMemory(poDevice, oBuffer, oMemory, ulMemoryOffset);
}

void cLogicalDevice::MapMemory(VkDeviceMemory& oMemory,
                               VkDeviceSize ulOffset,
                               VkDeviceSize ulSize,
                               VkMemoryMapFlags uiFlags,
                               void** ppData)
{
    vkMapMemory(poDevice, oMemory, ulOffset, ulSize, uiFlags, ppData);
}

void cLogicalDevice::UnmapMemory(VkDeviceMemory& oDeviceMemory)
{
    vkUnmapMemory(poDevice, oDeviceMemory);
}

void cLogicalDevice::FreeCommandBuffers(VkCommandPool& oCommandPool,
                                        uint uiCommandBufferCount,
                                        VkCommandBuffer* pCommandBuffers)
{
    vkFreeCommandBuffers(poDevice, oCommandPool, uiCommandBufferCount, pCommandBuffers);
}

bool cLogicalDevice::CreateDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo* pCreateInfo,
                                               VkAllocationCallbacks* pAllocator,
                                               VkDescriptorSetLayout* pSetLayout)
{
    return vkCreateDescriptorSetLayout(poDevice, pCreateInfo, pAllocator, pSetLayout) == VK_SUCCESS;
}

void cLogicalDevice::DestroyDescriptorSetLayout(VkDescriptorSetLayout& oDescriptorSetLayout,
                                                VkAllocationCallbacks* pAllocator)
{
    vkDestroyDescriptorSetLayout(poDevice, oDescriptorSetLayout, pAllocator);
}

bool cLogicalDevice::CreateDescriptorPool(VkDescriptorPoolCreateInfo* pCreateInfo,
                                          VkAllocationCallbacks* pAllocator,
                                          VkDescriptorPool* pDescriptorPool)
{
    return vkCreateDescriptorPool(poDevice, pCreateInfo, pAllocator, pDescriptorPool) == VK_SUCCESS;
}

void cLogicalDevice::DestroyDescriptorPool(VkDescriptorPool& oDescriptorPool, VkAllocationCallbacks* pAllocator)
{
    vkDestroyDescriptorPool(poDevice, oDescriptorPool, pAllocator);
}

bool cLogicalDevice::AllocateDescriptorSets(VkDescriptorSetAllocateInfo* pAllocateInfo,
                                            VkDescriptorSet* pDescriptorSets)
{
    return vkAllocateDescriptorSets(poDevice, pAllocateInfo, pDescriptorSets) == VK_SUCCESS;
}

void cLogicalDevice::FreeDescriptorSets(VkDescriptorPool& oDescriptorPool,
                                        uint uiDescriptorSetCount,
                                        VkDescriptorSet* pDescriptorSets)
{
    vkFreeDescriptorSets(poDevice, oDescriptorPool, uiDescriptorSetCount, pDescriptorSets);
}

void cLogicalDevice::UpdateDescriptorSets(uint uiDescriptorWriteCount,
                                          VkWriteDescriptorSet* pDescriptorWrites,
                                          uint uiDescriptorCopyCount,
                                          VkCopyDescriptorSet* pDescriptorCopies)
{
    vkUpdateDescriptorSets(poDevice, uiDescriptorWriteCount, pDescriptorWrites,
                           uiDescriptorCopyCount, pDescriptorCopies);
}

bool cLogicalDevice::CreateImage(VkImageCreateInfo* pCreateInfo, VkAllocationCallbacks* pAllocator, VkImage* pImage)
{
    return vkCreateImage(poDevice, pCreateInfo, pAllocator, pImage) == VK_SUCCESS;
}

void cLogicalDevice::GetImageMemoryRequirements(VkImage& oImage, VkMemoryRequirements* pMemoryRequirements)
{
    vkGetImageMemoryRequirements(poDevice, oImage, pMemoryRequirements);
}

void cLogicalDevice::BindImageMemory(VkImage& oImage, VkDeviceMemory& oMemory, VkDeviceSize ulMemoryOffset)
{
    vkBindImageMemory(poDevice, oImage, oMemory, ulMemoryOffset);
}

bool cLogicalDevice::CreateImageView(VkImageViewCreateInfo* pCreateInfo,
                                     VkAllocationCallbacks* pAllocator,
                                     VkImageView* pView)
{
    return vkCreateImageView(poDevice, pCreateInfo, pAllocator, pView) == VK_SUCCESS;
}


