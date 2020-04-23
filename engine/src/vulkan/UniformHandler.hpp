#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/geometry/BufferHelper.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include "vulkan/GraphicsPipeline.hpp"
#include "vulkan/texture/TextureHandler.hpp"

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

    void SetupUniformBuffers(uint uiCount, cTextureHandler* pTextureHandler);
    void UpdateUniformBuffers();

    uint GetDescriptorSetLayoutCount(void);
    VkDescriptorSetLayout* GetDescriptorSetLayouts(void);

    void CmdBindDescriptorSets(VkCommandBuffer& commandBuffer, VkPipelineLayout& oPipelineLayout, uint uiIndex);

private:
    void CreateUniformBuffers(uint uiCount);
    void CreateDescriptorPool();
    void CreateDescriptorSets(cTextureHandler* pTextureHandler);
};

cUniformHandler::cUniformHandler(cLogicalDevice* pLogicalDevice,
                                 cSwapChain* pSwapChain)
{
    ppLogicalDevice = pLogicalDevice;
    ppSwapChain = pSwapChain;

    VkDescriptorSetLayoutBinding tUBOLayoutBinding = {};
    tUBOLayoutBinding.binding = 0;
    tUBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    tUBOLayoutBinding.descriptorCount = 1;
    tUBOLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    tUBOLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding tSamplerLayoutBinding = {};
    tSamplerLayoutBinding.binding = 1;
    tSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    tSamplerLayoutBinding.descriptorCount = 1;
    tSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    tSamplerLayoutBinding.pImmutableSamplers = nullptr;

    std::array<VkDescriptorSetLayoutBinding, 2> atBindings = {tUBOLayoutBinding, tSamplerLayoutBinding};
    VkDescriptorSetLayoutCreateInfo tLayoutInfo = {};
    tLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    tLayoutInfo.bindingCount = atBindings.size();
    tLayoutInfo.pBindings = atBindings.data();

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

void cUniformHandler::SetupUniformBuffers(uint uiCount, cTextureHandler* pTextureHandler)
{
    CreateUniformBuffers(uiCount);
    CreateDescriptorPool();
    CreateDescriptorSets(pTextureHandler);
}

void cUniformHandler::CreateUniformBuffers(uint uiCount)
{
    VkDeviceSize bufferSize = sizeof(tUniformBufferObject);

    paoUniformBuffers.resize(uiCount);
    paoUniformBuffersMemory.resize(uiCount);

    for (uint i = 0; i < uiCount; i++)
    {
        cBufferHelper::CreateBuffer(ppLogicalDevice, bufferSize,
                                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    paoUniformBuffers[i], paoUniformBuffersMemory[i]);
    }
}

void cUniformHandler::UpdateUniformBuffers()
{
    //assert(uiImageIndex < paoUniformBuffers.size()); // image index must be within the bounds of the buffers list

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();


    void* data;
    for (uint i = 0; i < paoDescriptorSets.size(); i++)
    {
        VkDeviceMemory& oMemory = paoUniformBuffersMemory[i];

        tUniformBufferObject tUBO = {};

        tUBO.tModel = glm::rotate(
                glm::mat4(1.0f),
                (time * glm::radians(i == 0 ? 20.0f : -20.0f)),
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

        ppLogicalDevice->MapMemory(oMemory, 0, sizeof(tUBO), 0, &data);
        {
            memcpy(data, &tUBO, sizeof(tUBO));
        }
        ppLogicalDevice->UnmapMemory(oMemory);
    }
}

void cUniformHandler::CreateDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> atPoolSizes = {};
    atPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    atPoolSizes[0].descriptorCount = paoUniformBuffers.size();
    atPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    atPoolSizes[1].descriptorCount = paoUniformBuffers.size();

    VkDescriptorPoolCreateInfo tPoolInfo = {};
    tPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    tPoolInfo.poolSizeCount = atPoolSizes.size();
    tPoolInfo.pPoolSizes = atPoolSizes.data();

    tPoolInfo.maxSets = paoUniformBuffers.size();

    if (!ppLogicalDevice->CreateDescriptorPool(&tPoolInfo, nullptr, &poDescriptorPool))
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void cUniformHandler::CreateDescriptorSets(cTextureHandler* pTextureHandler)
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

        VkDescriptorImageInfo tImageInfo = {};
        tImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        tImageInfo.imageView = pTextureHandler->GetTexture(i)->GetView();
        tImageInfo.sampler = pTextureHandler->GetSampler();

        std::array<VkWriteDescriptorSet, 2> atDescriptorWrites = {};

        atDescriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        atDescriptorWrites[0].dstSet = paoDescriptorSets[i];
        atDescriptorWrites[0].dstBinding = 0;
        atDescriptorWrites[0].dstArrayElement = 0;
        atDescriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        atDescriptorWrites[0].descriptorCount = 1;
        atDescriptorWrites[0].pBufferInfo = &tBufferInfo;

        atDescriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        atDescriptorWrites[1].dstSet = paoDescriptorSets[i];
        atDescriptorWrites[1].dstBinding = 1;
        atDescriptorWrites[1].dstArrayElement = 0;
        atDescriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        atDescriptorWrites[1].descriptorCount = 1;
        atDescriptorWrites[1].pImageInfo = &tImageInfo;

        ppLogicalDevice->UpdateDescriptorSets(atDescriptorWrites.size(), atDescriptorWrites.data(),
                                              0, nullptr);
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
