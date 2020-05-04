#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/geometry/BufferHelper.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <vulkan/mesh/Mesh.hpp>
#include <vulkan/scene/Scene.hpp>
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

    void SetupUniformBuffers(cTextureHandler* pTextureHandler, cScene* pScene);
    void UpdateUniformBuffers(cScene* pScene);

    uint GetDescriptorSetLayoutCount(void);
    VkDescriptorSetLayout* GetDescriptorSetLayouts(void);

    void CmdBindDescriptorSets(VkCommandBuffer& commandBuffer, VkPipelineLayout& oPipelineLayout, uint uiIndex);

private:
    void CreateUniformBuffers(cScene* pScene);
    void CreateDescriptorPool();
    void CreateDescriptorSets(cTextureHandler* pTextureHandler, cScene* pScene);
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

void cUniformHandler::SetupUniformBuffers(cTextureHandler* pTextureHandler,
                                          cScene* pScene)
{
    CreateUniformBuffers(pScene);
    CreateDescriptorPool();
    CreateDescriptorSets(pTextureHandler, pScene);
}

void cUniformHandler::CreateUniformBuffers(cScene* pScene)
{
    VkDeviceSize bufferSize = sizeof(tUniformBufferObject);
    uint uiCount = pScene->GetObjectCount();

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

void cUniformHandler::UpdateUniformBuffers(cScene* pScene)
{
    assert(pScene != nullptr);

    static VkExtent2D tExtent = ppSwapChain->ptSwapChainExtent;
    static glm::mat4 oProjection = glm::perspective(
            glm::radians(45.0f),
            tExtent.width / (float) tExtent.height,
            0.1f, 100.0f);

    /*
     * TODO: Split camera matrices from object matrices.
     * Currently the camera matrices (projection and
     * view) are combined with the object (model) matrix
     * and updated for every object individually. These
     * two should be separated to improve performance.
     */

    void* data;
    uint uiIndex = 0;
    for (auto oObject : pScene->GetObjects())
    {
        tUniformBufferObject tUBO = {};

        // Set the model matrix of the object
        tUBO.tModel = oObject.second->GetModelMatrix();

        // Set the view matrix of the camera
        tUBO.tView = pScene->GetCamera().GetViewMatrix();

        // Set the projection matrix
        tUBO.tProjection = oProjection;
        tUBO.tProjection[1][1] *= -1; // invert the Y axis

        // Copy the data to memory
        VkDeviceMemory& oMemory = paoUniformBuffersMemory[uiIndex++];
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

void cUniformHandler::CreateDescriptorSets(cTextureHandler* pTextureHandler, cScene* pScene)
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

    uint uiIndex = 0;
    for (auto oObject : pScene->GetObjects())
    {
        VkDescriptorBufferInfo tBufferInfo = {};
        tBufferInfo.buffer = paoUniformBuffers[uiIndex];
        tBufferInfo.offset = 0;
        tBufferInfo.range = sizeof(tUniformBufferObject);

        VkDescriptorImageInfo tImageInfo = {};
        tImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        tImageInfo.imageView = oObject.second->GetMesh()->GetTexture()->GetView();
        tImageInfo.sampler = pTextureHandler->GetSampler();

        std::array<VkWriteDescriptorSet, 2> atDescriptorWrites = {};

        atDescriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        atDescriptorWrites[0].dstSet = paoDescriptorSets[uiIndex];
        atDescriptorWrites[0].dstBinding = 0;
        atDescriptorWrites[0].dstArrayElement = 0;
        atDescriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        atDescriptorWrites[0].descriptorCount = 1;
        atDescriptorWrites[0].pBufferInfo = &tBufferInfo;

        atDescriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        atDescriptorWrites[1].dstSet = paoDescriptorSets[uiIndex];
        atDescriptorWrites[1].dstBinding = 1;
        atDescriptorWrites[1].dstArrayElement = 0;
        atDescriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        atDescriptorWrites[1].descriptorCount = 1;
        atDescriptorWrites[1].pImageInfo = &tImageInfo;

        ppLogicalDevice->UpdateDescriptorSets(atDescriptorWrites.size(), atDescriptorWrites.data(),
                                              0, nullptr);

        uiIndex++;
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
