#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/PhysicalDevice.hpp>

class cLogicalDevice
{
private:
    // Logical device handle
    VkDevice poDevice = VK_NULL_HANDLE;

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
                            VkDescriptorSet* pDescriptorSets); // TODO: Descriptor sets are never freed?
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

    bool CreateSampler(VkSamplerCreateInfo* pCreateInfo,
                       VkAllocationCallbacks* pAllocator,
                       VkSampler* pSampler);
    void DestroySampler(VkSampler& oSampler,
                        VkAllocationCallbacks* pAllocator);


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
    uint graphicsQueueIndex = ptQueueIndices.oulGraphicsFamily.value();
    uint presentQueueIndex = ptQueueIndices.oulPresentFamily.value();
    std::vector<uint> uniqueQueueFamilies;
    uniqueQueueFamilies.push_back(graphicsQueueIndex);
    if (presentQueueIndex != graphicsQueueIndex)
    {
        uniqueQueueFamilies.push_back(presentQueueIndex);
    }

    // Struct with information about the physical device features we want to enable
    VkPhysicalDeviceFeatures tDeviceFeatures = {};
    tDeviceFeatures.samplerAnisotropy = VK_TRUE;

    for (uint uiQueueFamily : uniqueQueueFamilies)
    {
        queueCreateInfos.emplace_back(GetQueueCreateInfo(uiQueueFamily));
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
    assert(poDevice != VK_NULL_HANDLE);

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
    assert(poDevice != VK_NULL_HANDLE);

    vkWaitForFences(poDevice, uiFenceCount, pFences, bWaitAll, ulTimeout);
}

void cLogicalDevice::ResetFences(uint uiFenceCount, VkFence* pFences)
{
    assert(poDevice != VK_NULL_HANDLE);

    vkResetFences(poDevice, uiFenceCount, pFences);
}

void cLogicalDevice::WaitUntilIdle(void)
{
    assert(poDevice != VK_NULL_HANDLE);

    vkDeviceWaitIdle(poDevice);
}

bool cLogicalDevice::AllocateCommandBuffers(VkCommandBufferAllocateInfo* pAllocateInfo,
                                            VkCommandBuffer* pCommandBuffers)
{
    assert(poDevice != VK_NULL_HANDLE);

    return vkAllocateCommandBuffers(poDevice, pAllocateInfo, pCommandBuffers) == VK_SUCCESS;
}

bool cLogicalDevice::CreateCommandPool(VkCommandPoolCreateInfo* pCreateInfo,
                                       VkAllocationCallbacks* pAllocator,
                                       VkCommandPool* pCommandPool)
{
    assert(poDevice != VK_NULL_HANDLE);

    return vkCreateCommandPool(poDevice, pCreateInfo, pAllocator, pCommandPool) == VK_SUCCESS;
}

void cLogicalDevice::DestroyCommandPool(VkCommandPool& oCommandPool, VkAllocationCallbacks* pAllocator)
{
    assert(poDevice != VK_NULL_HANDLE);

    vkDestroyCommandPool(poDevice, oCommandPool, pAllocator);
}

bool cLogicalDevice::CreateShaderModule(VkShaderModuleCreateInfo* pCreateInfo,
                                        VkAllocationCallbacks* pAllocator,
                                        VkShaderModule* pShaderModule)
{
    assert(poDevice != VK_NULL_HANDLE);

    return vkCreateShaderModule(poDevice, pCreateInfo, pAllocator, pShaderModule) == VK_SUCCESS;
}

void cLogicalDevice::DestroyShaderModule(VkShaderModule& oShaderModule,
                                         VkAllocationCallbacks* pAllocator)
{
    assert(poDevice != VK_NULL_HANDLE);

    vkDestroyShaderModule(poDevice, oShaderModule, pAllocator);
}

bool cLogicalDevice::CreatePipelineLayout(VkPipelineLayoutCreateInfo* pCreateInfo,
                                          VkAllocationCallbacks* pAllocator,
                                          VkPipelineLayout* pPipelineLayout)
{
    assert(poDevice != VK_NULL_HANDLE);

    return vkCreatePipelineLayout(poDevice, pCreateInfo, pAllocator, pPipelineLayout) == VK_SUCCESS;
}

void cLogicalDevice::DestroyPipelineLayout(VkPipelineLayout& oPipelineLayout, VkAllocationCallbacks* pAllocator)
{
    assert(poDevice != VK_NULL_HANDLE);

    vkDestroyPipelineLayout(poDevice, oPipelineLayout, pAllocator);
}

bool cLogicalDevice::CreateGraphicsPipeline(uint uiCreateInfoCount,
                                            VkGraphicsPipelineCreateInfo* pCreateInfos,
                                            VkPipeline* pPipelines)
{
    assert(poDevice != VK_NULL_HANDLE);

    return vkCreateGraphicsPipelines(poDevice, VK_NULL_HANDLE, uiCreateInfoCount, pCreateInfos, nullptr, pPipelines)
           == VK_SUCCESS;
}

void cLogicalDevice::DestroyPipeline(VkPipeline& oPipeline, VkAllocationCallbacks* pAllocator)
{
    assert(poDevice != VK_NULL_HANDLE);

    vkDestroyPipeline(poDevice, oPipeline, pAllocator);
}

bool cLogicalDevice::CreateBuffer(VkBufferCreateInfo* pCreateInfo, VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer)
{
    assert(poDevice != VK_NULL_HANDLE);

    return vkCreateBuffer(poDevice, pCreateInfo, pAllocator, pBuffer) == VK_SUCCESS;
}

void cLogicalDevice::DestroyBuffer(VkBuffer& oBuffer, VkAllocationCallbacks* pAllocator)
{
    assert(poDevice != VK_NULL_HANDLE);

    vkDestroyBuffer(poDevice, oBuffer, pAllocator);
}

bool cLogicalDevice::AllocateMemory(VkMemoryAllocateInfo* pAllocateInfo,
                                    VkAllocationCallbacks* pAllocator,
                                    VkDeviceMemory* pMemory)
{
    assert(poDevice != VK_NULL_HANDLE);

    return vkAllocateMemory(poDevice, pAllocateInfo, pAllocator, pMemory) == VK_SUCCESS;
}

void cLogicalDevice::FreeMemory(VkDeviceMemory& oMemory, VkAllocationCallbacks* pAllocator)
{
    assert(poDevice != VK_NULL_HANDLE);

    vkFreeMemory(poDevice, oMemory, pAllocator);
}

void cLogicalDevice::BindBufferMemory(VkBuffer& oBuffer, VkDeviceMemory& oMemory, VkDeviceSize ulMemoryOffset)
{
    assert(poDevice != VK_NULL_HANDLE);

    vkBindBufferMemory(poDevice, oBuffer, oMemory, ulMemoryOffset);
}

void cLogicalDevice::MapMemory(VkDeviceMemory& oMemory,
                               VkDeviceSize ulOffset,
                               VkDeviceSize ulSize,
                               VkMemoryMapFlags uiFlags,
                               void** ppData)
{
    assert(poDevice != VK_NULL_HANDLE);

    vkMapMemory(poDevice, oMemory, ulOffset, ulSize, uiFlags, ppData);
}

void cLogicalDevice::UnmapMemory(VkDeviceMemory& oDeviceMemory)
{
    assert(poDevice != VK_NULL_HANDLE);

    vkUnmapMemory(poDevice, oDeviceMemory);
}

void cLogicalDevice::FreeCommandBuffers(VkCommandPool& oCommandPool,
                                        uint uiCommandBufferCount,
                                        VkCommandBuffer* pCommandBuffers)
{
    assert(poDevice != VK_NULL_HANDLE);

    vkFreeCommandBuffers(poDevice, oCommandPool, uiCommandBufferCount, pCommandBuffers);
}

bool cLogicalDevice::CreateDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo* pCreateInfo,
                                               VkAllocationCallbacks* pAllocator,
                                               VkDescriptorSetLayout* pSetLayout)
{
    assert(poDevice != VK_NULL_HANDLE);

    return vkCreateDescriptorSetLayout(poDevice, pCreateInfo, pAllocator, pSetLayout) == VK_SUCCESS;
}

void cLogicalDevice::DestroyDescriptorSetLayout(VkDescriptorSetLayout& oDescriptorSetLayout,
                                                VkAllocationCallbacks* pAllocator)
{
    assert(poDevice != VK_NULL_HANDLE);

    vkDestroyDescriptorSetLayout(poDevice, oDescriptorSetLayout, pAllocator);
}

bool cLogicalDevice::CreateDescriptorPool(VkDescriptorPoolCreateInfo* pCreateInfo,
                                          VkAllocationCallbacks* pAllocator,
                                          VkDescriptorPool* pDescriptorPool)
{
    assert(poDevice != VK_NULL_HANDLE);

    return vkCreateDescriptorPool(poDevice, pCreateInfo, pAllocator, pDescriptorPool) == VK_SUCCESS;
}

void cLogicalDevice::DestroyDescriptorPool(VkDescriptorPool& oDescriptorPool, VkAllocationCallbacks* pAllocator)
{
    assert(poDevice != VK_NULL_HANDLE);

    vkDestroyDescriptorPool(poDevice, oDescriptorPool, pAllocator);
}

bool cLogicalDevice::AllocateDescriptorSets(VkDescriptorSetAllocateInfo* pAllocateInfo,
                                            VkDescriptorSet* pDescriptorSets)
{
    assert(poDevice != VK_NULL_HANDLE);

    return vkAllocateDescriptorSets(poDevice, pAllocateInfo, pDescriptorSets) == VK_SUCCESS;
}

void cLogicalDevice::FreeDescriptorSets(VkDescriptorPool& oDescriptorPool,
                                        uint uiDescriptorSetCount,
                                        VkDescriptorSet* pDescriptorSets)
{
    assert(poDevice != VK_NULL_HANDLE);

    vkFreeDescriptorSets(poDevice, oDescriptorPool, uiDescriptorSetCount, pDescriptorSets);
}

void cLogicalDevice::UpdateDescriptorSets(uint uiDescriptorWriteCount,
                                          VkWriteDescriptorSet* pDescriptorWrites,
                                          uint uiDescriptorCopyCount,
                                          VkCopyDescriptorSet* pDescriptorCopies)
{
    assert(poDevice != VK_NULL_HANDLE);

    vkUpdateDescriptorSets(poDevice, uiDescriptorWriteCount, pDescriptorWrites,
                           uiDescriptorCopyCount, pDescriptorCopies);
}

bool cLogicalDevice::CreateImage(VkImageCreateInfo* pCreateInfo, VkAllocationCallbacks* pAllocator, VkImage* pImage)
{
    assert(poDevice != VK_NULL_HANDLE);

    return vkCreateImage(poDevice, pCreateInfo, pAllocator, pImage) == VK_SUCCESS;
}

void cLogicalDevice::GetImageMemoryRequirements(VkImage& oImage, VkMemoryRequirements* pMemoryRequirements)
{
    assert(poDevice != VK_NULL_HANDLE);

    vkGetImageMemoryRequirements(poDevice, oImage, pMemoryRequirements);
}

void cLogicalDevice::BindImageMemory(VkImage& oImage, VkDeviceMemory& oMemory, VkDeviceSize ulMemoryOffset)
{
    assert(poDevice != VK_NULL_HANDLE);

    vkBindImageMemory(poDevice, oImage, oMemory, ulMemoryOffset);
}

bool cLogicalDevice::CreateImageView(VkImageViewCreateInfo* pCreateInfo,
                                     VkAllocationCallbacks* pAllocator,
                                     VkImageView* pView)
{
    assert(poDevice != VK_NULL_HANDLE);

    return vkCreateImageView(poDevice, pCreateInfo, pAllocator, pView) == VK_SUCCESS;
}

bool cLogicalDevice::CreateSampler(VkSamplerCreateInfo* pCreateInfo,
                                   VkAllocationCallbacks* pAllocator,
                                   VkSampler* pSampler)
{
    assert(poDevice != VK_NULL_HANDLE);

    return vkCreateSampler(poDevice, pCreateInfo, pAllocator, pSampler) == VK_SUCCESS;
}

void cLogicalDevice::DestroySampler(VkSampler& oSampler,
                                    VkAllocationCallbacks* pAllocator)
{
    assert(poDevice != VK_NULL_HANDLE);

    vkDestroySampler(poDevice, oSampler, pAllocator);
}


