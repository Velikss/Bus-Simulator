#pragma once

#include <pch.hpp>
#include <vulkan/uniform/UniformHandler.hpp>
#include <vulkan/SwapChain.hpp>
#include "Font.hpp"

class cOverlayUniformHandler : public iUniformHandler
{
private:
    cLogicalDevice* ppLogicalDevice;

    cFont* ppFont;

    VkDescriptorSetLayout poDescriptorSetLayout;

    VkDescriptorPool poDescriptorPool;
    VkDescriptorSet poDescriptorSet;

public:
    cOverlayUniformHandler(cLogicalDevice* pLogicalDevice,
                           cFont* pFont);
    ~cOverlayUniformHandler();

    void SetupUniformBuffers(cTextureHandler* pTextureHandler, cScene* pScene) override;
    void UpdateUniformBuffers(cScene* pScene) override;

    uint GetDescriptorSetLayoutCount(void) override;
    VkDescriptorSetLayout* GetDescriptorSetLayouts(void) override;

    void CmdBindDescriptorSets(VkCommandBuffer& commandBuffer,
                               VkPipelineLayout& oPipelineLayout,
                               uint uiIndex) override;

private:
    void CreateDescriptorPool();
    void CreateDescriptorSet();
};

cOverlayUniformHandler::cOverlayUniformHandler(cLogicalDevice* pLogicalDevice, cFont* pFont)
{
    ppLogicalDevice = pLogicalDevice;
    ppFont = pFont;

    VkDescriptorSetLayoutBinding tSamplerLayoutBinding = {};
    tSamplerLayoutBinding.binding = 0;
    tSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    tSamplerLayoutBinding.descriptorCount = 1;
    tSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    tSamplerLayoutBinding.pImmutableSamplers = nullptr;

    std::array<VkDescriptorSetLayoutBinding, 1> atBindings = {tSamplerLayoutBinding};

    VkDescriptorSetLayoutCreateInfo tLayoutInfo = {};
    tLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    tLayoutInfo.bindingCount = atBindings.size();
    tLayoutInfo.pBindings = atBindings.data();

    if (!pLogicalDevice->CreateDescriptorSetLayout(&tLayoutInfo, nullptr, &poDescriptorSetLayout))
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

cOverlayUniformHandler::~cOverlayUniformHandler()
{
    ppLogicalDevice->DestroyDescriptorSetLayout(poDescriptorSetLayout, nullptr);
    ppLogicalDevice->DestroyDescriptorPool(poDescriptorPool, nullptr);
}

void cOverlayUniformHandler::SetupUniformBuffers(cTextureHandler* pTextureHandler, cScene* pScene)
{
    CreateDescriptorPool();
    CreateDescriptorSet();
}

void cOverlayUniformHandler::CreateDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 1> atPoolSizes = {};
    atPoolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    atPoolSizes[0].descriptorCount = 1;

    VkDescriptorPoolCreateInfo tPoolInfo = {};
    tPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    tPoolInfo.poolSizeCount = atPoolSizes.size();
    tPoolInfo.pPoolSizes = atPoolSizes.data();

    tPoolInfo.maxSets = 1;

    if (!ppLogicalDevice->CreateDescriptorPool(&tPoolInfo, nullptr, &poDescriptorPool))
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void cOverlayUniformHandler::CreateDescriptorSet()
{
    std::vector<VkDescriptorSetLayout> aoLayouts(1, poDescriptorSetLayout);

    VkDescriptorSetAllocateInfo tAllocInfo = {};
    tAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

    tAllocInfo.descriptorPool = poDescriptorPool;
    tAllocInfo.descriptorSetCount = aoLayouts.size();
    tAllocInfo.pSetLayouts = aoLayouts.data();

    if (!ppLogicalDevice->AllocateDescriptorSets(&tAllocInfo, &poDescriptorSet))
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    VkDescriptorImageInfo tImageInfo = {};
    tImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    tImageInfo.imageView = ppFont->poFontImageView;
    tImageInfo.sampler = ppFont->poFontImageSampler;

    std::array<VkWriteDescriptorSet, 1> atDescriptorWrites = {};
    atDescriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    atDescriptorWrites[0].dstSet = poDescriptorSet;
    atDescriptorWrites[0].dstBinding = 0;
    atDescriptorWrites[0].dstArrayElement = 0;
    atDescriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    atDescriptorWrites[0].descriptorCount = 1;
    atDescriptorWrites[0].pImageInfo = &tImageInfo;

    ppLogicalDevice->UpdateDescriptorSets(atDescriptorWrites.size(), atDescriptorWrites.data(),
                                          0, nullptr);
}

void cOverlayUniformHandler::UpdateUniformBuffers(cScene* pScene)
{
}

uint cOverlayUniformHandler::GetDescriptorSetLayoutCount(void)
{
    return 1;
}

VkDescriptorSetLayout* cOverlayUniformHandler::GetDescriptorSetLayouts(void)
{
    return &poDescriptorSetLayout;
}

void cOverlayUniformHandler::CmdBindDescriptorSets(VkCommandBuffer& commandBuffer,
                                                   VkPipelineLayout& oPipelineLayout,
                                                   uint uiIndex)
{
    vkCmdBindDescriptorSets(commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            oPipelineLayout, 0, 1,
                            &poDescriptorSet,
                            0, nullptr);
}
