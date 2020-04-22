#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/BufferHelper.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include "vulkan/GraphicsPipeline.hpp"

struct tUniformBufferObject
{
    glm::mat4 tModel;
    glm::mat4 tView;
    glm::mat4 tProjection;
};

class cUniformHandler
{
private:
    cLogicalDevice* ppLogicalDevice;
    cSwapChain* ppSwapChain;

    VkDescriptorSetLayout poDescriptorSetLayout;

    std::vector<VkBuffer> paoUniformBuffers;
    std::vector<VkDeviceMemory> paoUniformBuffersMemory;

    VkDescriptorPool poDescriptorPool;
    std::vector<VkDescriptorSet> paoDescriptorSets;

public:
    cUniformHandler(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain);
    ~cUniformHandler(void);

    void SetupUniformBuffers(void);
    void UpdateUniformBuffers(uint uiImageIndex);

    uint GetDescriptorSetLayoutCount(void);
    VkDescriptorSetLayout* GetDescriptorSetLayouts(void);

    void CmdBindDescriptorSets(VkCommandBuffer& commandBuffer, VkPipelineLayout& oPipelineLayout, uint uiIndex);

private:
    void CreateUniformBuffers(void);
    void CreateDescriptorPool(void);
    void CreateDescriptorSets(void);
};

cUniformHandler::cUniformHandler(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain)
{
    ppLogicalDevice = pLogicalDevice;
    ppSwapChain = pSwapChain;

    VkDescriptorSetLayoutBinding tUBOLayoutBinding = {};
    tUBOLayoutBinding.binding = 0;
    tUBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    tUBOLayoutBinding.descriptorCount = 1;

    tUBOLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    tUBOLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo tLayoutInfo = {};
    tLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    tLayoutInfo.bindingCount = 1;
    tLayoutInfo.pBindings = &tUBOLayoutBinding;

    if (!pLogicalDevice->CreateDescriptorSetLayout(&tLayoutInfo, nullptr, &poDescriptorSetLayout))
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

cUniformHandler::~cUniformHandler()
{
    ppLogicalDevice->DestroyDescriptorSetLayout(poDescriptorSetLayout, nullptr);

    for (size_t i = 0; i < paoUniformBuffers.size(); i++)
    {
        ppLogicalDevice->DestroyBuffer(paoUniformBuffers[i], nullptr);
        ppLogicalDevice->FreeMemory(paoUniformBuffersMemory[i], nullptr);
    }

    ppLogicalDevice->DestroyDescriptorPool(poDescriptorPool, nullptr);
}

void cUniformHandler::SetupUniformBuffers(void)
{
    CreateUniformBuffers();
    CreateDescriptorPool();
    CreateDescriptorSets();
}

void cUniformHandler::CreateUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(tUniformBufferObject);

    uint uiBufferCount = ppSwapChain->GetFramebufferSize();
    paoUniformBuffers.resize(uiBufferCount);
    paoUniformBuffersMemory.resize(uiBufferCount);

    for (uint i = 0; i < uiBufferCount; i++)
    {
        cBufferHelper::CreateBuffer(ppLogicalDevice, bufferSize,
                                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    paoUniformBuffers[i], paoUniformBuffersMemory[i]);
    }
}

void cUniformHandler::UpdateUniformBuffers(uint uiImageIndex)
{
    assert(uiImageIndex < paoUniformBuffers.size()); // image index must be within the bounds of the buffers list

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    tUniformBufferObject tUBO = {};

    tUBO.tModel = glm::rotate(
            glm::mat4(1.0f),
            time * glm::radians(90.0f),
            glm::vec3(0.0f, 0.0f, 1.0f));

    tUBO.tView = glm::lookAt(
            glm::vec3(2.0f, 2.0f, 2.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f));

    VkExtent2D tExtent = ppSwapChain->ptSwapChainExtent;
    tUBO.tProjection = glm::perspective(
            glm::radians(45.0f),
            tExtent.width / (float) tExtent.height,
            0.1f, 10.0f);
    tUBO.tProjection[1][1] *= -1; // invert the Y axis

    void* data;
    ppLogicalDevice->MapMemory(paoUniformBuffersMemory[uiImageIndex], 0, sizeof(tUBO), 0, &data);
    {
        memcpy(data, &tUBO, sizeof(tUBO));
    }
    ppLogicalDevice->UnmapMemory(paoUniformBuffersMemory[uiImageIndex]);
}

void cUniformHandler::CreateDescriptorPool(void)
{
    VkDescriptorPoolSize tPoolSize = {};
    tPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    tPoolSize.descriptorCount = paoUniformBuffers.size();

    VkDescriptorPoolCreateInfo tPoolInfo{};
    tPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    tPoolInfo.poolSizeCount = 1;
    tPoolInfo.pPoolSizes = &tPoolSize;

    tPoolInfo.maxSets = paoUniformBuffers.size();

    if (!ppLogicalDevice->CreateDescriptorPool(&tPoolInfo, nullptr, &poDescriptorPool))
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void cUniformHandler::CreateDescriptorSets(void)
{
    std::vector<VkDescriptorSetLayout> aoLayouts(paoUniformBuffers.size(), poDescriptorSetLayout);

    VkDescriptorSetAllocateInfo tAllocInfo = {};
    tAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

    tAllocInfo.descriptorPool = poDescriptorPool;
    tAllocInfo.descriptorSetCount = paoUniformBuffers.size();
    tAllocInfo.pSetLayouts = aoLayouts.data();

    paoDescriptorSets.resize(paoUniformBuffers.size());
    if (!ppLogicalDevice->AllocateDescriptorSets(&tAllocInfo, paoDescriptorSets.data()))
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (uint i = 0; i < paoDescriptorSets.size(); i++)
    {
        VkDescriptorBufferInfo tBufferInfo = {};
        tBufferInfo.buffer = paoUniformBuffers[i];
        tBufferInfo.offset = 0;
        tBufferInfo.range = sizeof(tUniformBufferObject);

        VkWriteDescriptorSet tDescriptorWrite = {};
        tDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        tDescriptorWrite.dstSet = paoDescriptorSets[i];
        tDescriptorWrite.dstBinding = 0;
        tDescriptorWrite.dstArrayElement = 0;

        tDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        tDescriptorWrite.descriptorCount = 1;

        tDescriptorWrite.pBufferInfo = &tBufferInfo;

        ppLogicalDevice->UpdateDescriptorSets(1, &tDescriptorWrite, 0, nullptr);
    }
}

uint cUniformHandler::GetDescriptorSetLayoutCount(void)
{
    return 1;
}

VkDescriptorSetLayout* cUniformHandler::GetDescriptorSetLayouts(void)
{
    return &poDescriptorSetLayout;
}

void cUniformHandler::CmdBindDescriptorSets(VkCommandBuffer& commandBuffer,
                                            VkPipelineLayout& oPipelineLayout,
                                            uint uiIndex)
{
    vkCmdBindDescriptorSets(commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            oPipelineLayout, 0, 1,
                            &paoDescriptorSets[uiIndex],
                            0, nullptr);
}
