#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/geometry/BufferHelper.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/mesh/Mesh.hpp>
#include <vulkan/scene/Scene.hpp>
#include <vulkan/uniform/UniformHandler.hpp>
#include <vulkan/SwapChain.hpp>
#include <vulkan/scene/LightObject.hpp>

// Struct with information about a light source
struct tLight
{
    glm::vec4 tPosition;
    glm::vec3 tColor;
    float fRadius;
};

// Struct with information about the lighting
struct tLightsInfo
{
    glm::vec4 tViewPos;
    float fAmbientLight;
    uint uiLightsCount;
};

class cLightingUniformHandler : public iUniformHandler
{
private:
    cLogicalDevice* ppLogicalDevice;
    cSwapChain* ppSwapChain;

    VkDescriptorSetLayout poDescriptorSetLayout;

    VkBuffer poUniformBuffer;
    VkDeviceMemory poUniformBufferMemory;

    VkDescriptorPool poDescriptorPool;
    VkDescriptorSet poDescriptorSet;

    uint puiLightsCount;
    size_t puiLightsMemorySize;

public:
    cLightingUniformHandler(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain);
    ~cLightingUniformHandler() override;

    void SetupUniformBuffers(cTextureHandler* pTextureHandler, cScene* pScene) override;
    void UpdateUniformBuffers(cScene* pScene) override;

    uint GetDescriptorSetLayoutCount(void) override;
    VkDescriptorSetLayout* GetDescriptorSetLayouts(void) override;

    void CmdBindDescriptorSets(VkCommandBuffer& commandBuffer,
                               VkPipelineLayout& oPipelineLayout,
                               uint uiIndex) override;

private:
    void CreateUniformBuffers(cScene* pScene);
    void CreateDescriptorPool();
    void CreateDescriptorSets(cTextureHandler* pTextureHandler, cScene* pScene);
};

cLightingUniformHandler::cLightingUniformHandler(cLogicalDevice* pLogicalDevice,
                                                 cSwapChain* pSwapChain)
{
    ppLogicalDevice = pLogicalDevice;
    ppSwapChain = pSwapChain;

    std::array<VkDescriptorSetLayoutBinding, 4> atLayoutBindings;

    atLayoutBindings[0].binding = 0;
    atLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    atLayoutBindings[0].descriptorCount = 1;
    atLayoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    atLayoutBindings[0].pImmutableSamplers = nullptr;

    atLayoutBindings[1].binding = 1;
    atLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    atLayoutBindings[1].descriptorCount = 1;
    atLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    atLayoutBindings[1].pImmutableSamplers = nullptr;

    atLayoutBindings[2].binding = 2;
    atLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    atLayoutBindings[2].descriptorCount = 1;
    atLayoutBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    atLayoutBindings[2].pImmutableSamplers = nullptr;

    atLayoutBindings[3].binding = 3;
    atLayoutBindings[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    atLayoutBindings[3].descriptorCount = 1;
    atLayoutBindings[3].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    atLayoutBindings[3].pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo tCameraLayoutInfo = {};
    tCameraLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    tCameraLayoutInfo.bindingCount = atLayoutBindings.size();
    tCameraLayoutInfo.pBindings = atLayoutBindings.data();

    if (!pLogicalDevice->CreateDescriptorSetLayout(&tCameraLayoutInfo, nullptr, &poDescriptorSetLayout))
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}


cLightingUniformHandler::~cLightingUniformHandler()
{
    ppLogicalDevice->DestroyDescriptorSetLayout(poDescriptorSetLayout, nullptr);

    ppLogicalDevice->DestroyBuffer(poUniformBuffer, nullptr);
    ppLogicalDevice->FreeMemory(poUniformBufferMemory, nullptr);

    ppLogicalDevice->DestroyDescriptorPool(poDescriptorPool, nullptr);
}

void cLightingUniformHandler::SetupUniformBuffers(cTextureHandler* pTextureHandler,
                                                  cScene* pScene)
{
    CreateUniformBuffers(pScene);
    CreateDescriptorPool();
    CreateDescriptorSets(pTextureHandler, pScene);
}

void cLightingUniformHandler::CreateUniformBuffers(cScene* pScene)
{
    puiLightsCount = 0;
    for (auto pObject : pScene->GetObjects())
    {
        if (instanceof<cLightObject>(pObject.second))
        {
            puiLightsCount++;
        }
    }

    // The code below assumes that tLightsInfo is less than or
    // equal to 32 bytes, and tLight is exactly 32 bytes.
    static_assert(sizeof(tLightsInfo) <= 32, "Alignment code needs to be updated when tLightsInfo changes");
    static_assert(sizeof(tLight) == 32, "Alignment code needs to be updated when tLight changes");

    puiLightsMemorySize = 32 + (sizeof(tLight) * puiLightsCount);
    cBufferHelper::CreateBuffer(ppLogicalDevice, puiLightsMemorySize,
                                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                poUniformBuffer, poUniformBufferMemory);

    ENGINE_LOG("Allocated " << puiLightsMemorySize << " bytes for " << puiLightsCount << " scene lights");
}

void cLightingUniformHandler::UpdateUniformBuffers(cScene* pScene)
{
    // We don't want to do anything before the scene is loaded
    if (pScene == nullptr) return;

    // Get the camera
    Camera* pCamera = &pScene->GetCamera();

    // Lights info consists of amount of lights, camera pos and ambient light level
    tLightsInfo tLightsInfo = {};
    tLightsInfo.uiLightsCount = puiLightsCount;
    tLightsInfo.tViewPos = glm::vec4(pCamera->GetPosition(), 0.0f) * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
    tLightsInfo.fAmbientLight = pScene->pfAmbientLight;

    // Loop over all the objects in the scene. If the object is a
    //  light, add it to the list of lights
    std::vector<tLight> atLights;
    for (auto pObject : pScene->GetObjects())
    {
        if (instanceof<cLightObject>(pObject.second))
        {
            cLightObject* pLight = dynamic_cast<cLightObject*>(pObject.second);
            tLight tLight = {};
            tLight.tPosition = glm::vec4(pLight->GetPosition(), 0.0f);
            tLight.tColor = pLight->GetColor();
            tLight.fRadius = pLight->GetRadius();
            atLights.push_back(tLight);
        }
    }

    // The code below assumes that tLightsInfo is less than or
    // equal to 32 bytes, and tLight is exactly 32 bytes.
    static_assert(sizeof(tLightsInfo) <= 32, "Alignment code needs to be updated when tLightsInfo changes");
    static_assert(sizeof(tLight) == 32, "Alignment code needs to be updated when tLight changes");

    // Copy the data to memory
    byte* pMappedMemory;
    ppLogicalDevice->MapMemory(poUniformBufferMemory, 0, puiLightsMemorySize,
                               0, reinterpret_cast<void**>(&pMappedMemory));
    {
        memcpy(pMappedMemory, &tLightsInfo, sizeof(tLightsInfo));
        memcpy(pMappedMemory + 32, &atLights[0], puiLightsMemorySize - 32);
    }
    ppLogicalDevice->UnmapMemory(poUniformBufferMemory);
}

void cLightingUniformHandler::CreateDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> atPoolSizes = {};
    atPoolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    atPoolSizes[0].descriptorCount = 1;
    atPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    atPoolSizes[1].descriptorCount = 3;

    VkDescriptorPoolCreateInfo tPoolInfo = {};
    tPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    tPoolInfo.poolSizeCount = atPoolSizes.size();
    tPoolInfo.pPoolSizes = atPoolSizes.data();

    tPoolInfo.maxSets = 4;

    if (!ppLogicalDevice->CreateDescriptorPool(&tPoolInfo, nullptr, &poDescriptorPool))
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void cLightingUniformHandler::CreateDescriptorSets(cTextureHandler* pTextureHandler, cScene* pScene)
{
    VkDescriptorSetAllocateInfo tAllocateInfo = {};
    tAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

    tAllocateInfo.descriptorPool = poDescriptorPool;
    tAllocateInfo.descriptorSetCount = 1;
    tAllocateInfo.pSetLayouts = &poDescriptorSetLayout;

    if (!ppLogicalDevice->AllocateDescriptorSets(&tAllocateInfo, &poDescriptorSet))
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    VkDescriptorBufferInfo tCameraBufferInfo = {};
    tCameraBufferInfo.buffer = poUniformBuffer;
    tCameraBufferInfo.offset = 0;
    tCameraBufferInfo.range = VK_WHOLE_SIZE;

    VkDescriptorImageInfo tPositionInfo = {};
    tPositionInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    tPositionInfo.imageView = ppSwapChain->GetAttachment(0).oView;
    tPositionInfo.sampler = ppSwapChain->GetSampler();

    VkDescriptorImageInfo tNormalInfo = {};
    tNormalInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    tNormalInfo.imageView = ppSwapChain->GetAttachment(1).oView;
    tNormalInfo.sampler = ppSwapChain->GetSampler();

    VkDescriptorImageInfo tAlbedoInfo = {};
    tAlbedoInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    tAlbedoInfo.imageView = ppSwapChain->GetAttachment(2).oView;
    tAlbedoInfo.sampler = ppSwapChain->GetSampler();

    std::array<VkWriteDescriptorSet, 4> atDescriptorWrites = {};

    atDescriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    atDescriptorWrites[0].dstSet = poDescriptorSet;
    atDescriptorWrites[0].dstBinding = 0;
    atDescriptorWrites[0].dstArrayElement = 0;
    atDescriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    atDescriptorWrites[0].descriptorCount = 1;
    atDescriptorWrites[0].pBufferInfo = &tCameraBufferInfo;

    atDescriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    atDescriptorWrites[1].dstSet = poDescriptorSet;
    atDescriptorWrites[1].dstBinding = 1;
    atDescriptorWrites[1].dstArrayElement = 0;
    atDescriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    atDescriptorWrites[1].descriptorCount = 1;
    atDescriptorWrites[1].pImageInfo = &tPositionInfo;

    atDescriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    atDescriptorWrites[2].dstSet = poDescriptorSet;
    atDescriptorWrites[2].dstBinding = 2;
    atDescriptorWrites[2].dstArrayElement = 0;
    atDescriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    atDescriptorWrites[2].descriptorCount = 1;
    atDescriptorWrites[2].pImageInfo = &tNormalInfo;

    atDescriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    atDescriptorWrites[3].dstSet = poDescriptorSet;
    atDescriptorWrites[3].dstBinding = 3;
    atDescriptorWrites[3].dstArrayElement = 0;
    atDescriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    atDescriptorWrites[3].descriptorCount = 1;
    atDescriptorWrites[3].pImageInfo = &tAlbedoInfo;

    ppLogicalDevice->UpdateDescriptorSets(atDescriptorWrites.size(), atDescriptorWrites.data(),
                                          0, nullptr);
}

uint cLightingUniformHandler::GetDescriptorSetLayoutCount(void)
{
    return 1;
}

VkDescriptorSetLayout* cLightingUniformHandler::GetDescriptorSetLayouts(void)
{
    return &poDescriptorSetLayout;
}

void cLightingUniformHandler::CmdBindDescriptorSets(VkCommandBuffer& commandBuffer,
                                                    VkPipelineLayout& oPipelineLayout,
                                                    uint uiIndex)
{
    vkCmdBindDescriptorSets(commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            oPipelineLayout, 0,
                            1, &poDescriptorSet,
                            0, nullptr);
}
