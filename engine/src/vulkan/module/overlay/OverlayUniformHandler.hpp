#pragma once

#include <pch.hpp>
#include <vulkan/uniform/UniformHandler.hpp>
#include <vulkan/SwapChain.hpp>
#include <vulkan/module/overlay/text/Font.hpp>

struct tOverlayUniformObject
{
    glm::vec3 color;
};

class cOverlayUniformHandler : public iUniformHandler
{
private:
    cLogicalDevice* ppLogicalDevice;

    cFont* ppFont;

    VkDescriptorSetLayout poDescriptorSetLayout;

    VkDescriptorPool poDescriptorPool;
    VkDescriptorSet poDescriptorSet;

    VkBuffer poBuffer;
    VkDeviceMemory poBufferMemory;

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
    void CreateUniformBuffers();
    void CreateDescriptorPool();
    void CreateDescriptorSet();
};

cOverlayUniformHandler::cOverlayUniformHandler(cLogicalDevice* pLogicalDevice, cFont* pFont)
{
    assert(pLogicalDevice != nullptr);
    assert(pFont != nullptr);

    ppLogicalDevice = pLogicalDevice;
    ppFont = pFont;

    VkDescriptorSetLayoutBinding tSamplerLayoutBinding = {};
    tSamplerLayoutBinding.binding = 0;
    tSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    tSamplerLayoutBinding.descriptorCount = 1;
    tSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    tSamplerLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding tDataLayoutBinding = {};
    tDataLayoutBinding.binding = 3;
    tDataLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    tDataLayoutBinding.descriptorCount = 1;
    tDataLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    tDataLayoutBinding.pImmutableSamplers = nullptr;

    std::array<VkDescriptorSetLayoutBinding, 2> atBindings = {tSamplerLayoutBinding, tDataLayoutBinding};

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
    ppLogicalDevice->DestroyBuffer(poBuffer, nullptr);
    ppLogicalDevice->FreeMemory(poBufferMemory, nullptr);
}

void cOverlayUniformHandler::SetupUniformBuffers(cTextureHandler* pTextureHandler, cScene* pScene)
{
    CreateUniformBuffers();
    CreateDescriptorPool();
    CreateDescriptorSet();
}

void cOverlayUniformHandler::CreateUniformBuffers()
{
    cBufferHelper::CreateBuffer(ppLogicalDevice, sizeof(tOverlayUniformObject),
                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                poBuffer, poBufferMemory);
}

void cOverlayUniformHandler::CreateDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> atPoolSizes = {};
    atPoolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    atPoolSizes[0].descriptorCount = 1;
    atPoolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    atPoolSizes[1].descriptorCount = 1;

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

    VkDescriptorBufferInfo tBufferInfo = {};
    tBufferInfo.buffer = poBuffer;
    tBufferInfo.offset = 0;
    tBufferInfo.range = sizeof(tOverlayUniformObject);

    std::array<VkWriteDescriptorSet, 2> atDescriptorWrites = {};
    atDescriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    atDescriptorWrites[0].dstSet = poDescriptorSet;
    atDescriptorWrites[0].dstBinding = 0;
    atDescriptorWrites[0].dstArrayElement = 0;
    atDescriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    atDescriptorWrites[0].descriptorCount = 1;
    atDescriptorWrites[0].pImageInfo = &tImageInfo;

    atDescriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    atDescriptorWrites[1].dstSet = poDescriptorSet;
    atDescriptorWrites[1].dstBinding = 3;
    atDescriptorWrites[1].dstArrayElement = 0;
    atDescriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    atDescriptorWrites[1].descriptorCount = 1;
    atDescriptorWrites[1].pBufferInfo = &tBufferInfo;

    ppLogicalDevice->UpdateDescriptorSets(atDescriptorWrites.size(), atDescriptorWrites.data(),
                                          0, nullptr);
}

void cOverlayUniformHandler::UpdateUniformBuffers(cScene* pScene)
{
    tOverlayUniformObject tObject = {};

    // If no scene is loaded, just use white
    tObject.color = glm::vec3(pScene == nullptr ? glm::vec3(1, 1, 1) : pScene->color);

    void* data;
    ppLogicalDevice->MapMemory(poBufferMemory, 0, sizeof(tObject), 0, &data);
    {
        memcpy(data, &tObject, sizeof(tObject));
    }
    ppLogicalDevice->UnmapMemory(poBufferMemory);
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
