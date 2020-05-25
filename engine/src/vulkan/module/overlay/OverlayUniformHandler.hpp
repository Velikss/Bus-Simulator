#pragma once

#include <pch.hpp>
#include <vulkan/uniform/UniformHandler.hpp>
#include <vulkan/swapchain/SwapChain.hpp>
#include <vulkan/module/overlay/text/Font.hpp>

struct tOverlayUniformObject
{
    glm::vec3 color;
};

struct tOverlayElementObject
{
    glm::mat4 tMatrix;
};

class cOverlayUniformHandler : public iUniformHandler
{
private:
    cLogicalDevice* ppLogicalDevice;

    cFont* ppFont;
    cWindow* ppWindow;

    VkDescriptorSetLayout poDescriptorSetLayout;
    VkDescriptorSetLayout poElementDescriptorSetLayout;
    VkDescriptorSetLayout paoDescriptorSetLayouts[2];

    VkBuffer poBuffer;
    VkDeviceMemory poBufferMemory;
    std::vector<VkBuffer> paoElementUniformBuffers;
    std::vector<VkDeviceMemory> paoElementUniformBuffersMemory;

    VkDescriptorPool poDescriptorPool;
    VkDescriptorSet poDescriptorSet;
    std::vector<VkDescriptorSet> paoElementDescriptorSets;

    VkDescriptorSet paoCurrentDescriptorSets[2];

public:
    cOverlayUniformHandler(cLogicalDevice* pLogicalDevice,
                           cFont* pFont, cWindow* pWindow);
    ~cOverlayUniformHandler();

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
    void CreateDescriptorSet(cScene* pScene);

    void CopyToDeviceMemory(VkDeviceMemory& oDeviceMemory, void* pData, uint uiDataSize);
};

cOverlayUniformHandler::cOverlayUniformHandler(cLogicalDevice* pLogicalDevice, cFont* pFont, cWindow* pWindow)
{
    assert(pLogicalDevice != nullptr);
    assert(pFont != nullptr);
    assert(pWindow != nullptr);

    ppLogicalDevice = pLogicalDevice;
    ppFont = pFont;
    ppWindow = pWindow;

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

    VkDescriptorSetLayoutBinding tElementTextureLayoutBinding = {};
    tElementTextureLayoutBinding.binding = 0;
    tElementTextureLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    tElementTextureLayoutBinding.descriptorCount = 1;
    tElementTextureLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    tElementTextureLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding tElementDataLayoutBinding = {};
    tElementDataLayoutBinding.binding = 1;
    tElementDataLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    tElementDataLayoutBinding.descriptorCount = 1;
    tElementDataLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    tElementDataLayoutBinding.pImmutableSamplers = nullptr;

    std::array<VkDescriptorSetLayoutBinding, 2> atElementBindings = {tElementTextureLayoutBinding,
                                                                     tElementDataLayoutBinding};

    VkDescriptorSetLayoutCreateInfo tElementLayoutInfo = {};
    tElementLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    tElementLayoutInfo.bindingCount = atElementBindings.size();
    tElementLayoutInfo.pBindings = atElementBindings.data();

    if (!pLogicalDevice->CreateDescriptorSetLayout(&tElementLayoutInfo, nullptr, &poElementDescriptorSetLayout))
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    paoDescriptorSetLayouts[0] = poDescriptorSetLayout;
    paoDescriptorSetLayouts[1] = poElementDescriptorSetLayout;
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
    CreateUniformBuffers(pScene);
    CreateDescriptorPool();
    CreateDescriptorSet(pScene);
}

void cOverlayUniformHandler::CreateUniformBuffers(cScene* pScene)
{
    cBufferHelper::CreateBuffer(ppLogicalDevice, sizeof(tOverlayUniformObject),
                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                poBuffer, poBufferMemory);

    VkDeviceSize bufferSize = sizeof(tOverlayElementObject);
    uint uiCount = pScene->GetOverlay().size();

    paoElementUniformBuffers.resize(uiCount);
    paoElementUniformBuffersMemory.resize(uiCount);
    for (uint i = 0; i < uiCount; i++)
    {
        cBufferHelper::CreateBuffer(ppLogicalDevice, bufferSize,
                                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    paoElementUniformBuffers[i], paoElementUniformBuffersMemory[i]);
    }
}

void cOverlayUniformHandler::CreateDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> atPoolSizes = {};
    atPoolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    atPoolSizes[0].descriptorCount = 1 + paoElementUniformBuffers.size();
    atPoolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    atPoolSizes[1].descriptorCount = 1 + paoElementUniformBuffers.size();

    VkDescriptorPoolCreateInfo tPoolInfo = {};
    tPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    tPoolInfo.poolSizeCount = atPoolSizes.size();
    tPoolInfo.pPoolSizes = atPoolSizes.data();

    tPoolInfo.maxSets = 1 + paoElementUniformBuffers.size();

    if (!ppLogicalDevice->CreateDescriptorPool(&tPoolInfo, nullptr, &poDescriptorPool))
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void cOverlayUniformHandler::CreateDescriptorSet(cScene* pScene)
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

    std::vector<VkDescriptorSetLayout> aoElementLayouts(paoElementUniformBuffers.size(), poElementDescriptorSetLayout);

    VkDescriptorSetAllocateInfo tElementAllocInfo = {};
    tElementAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

    tElementAllocInfo.descriptorPool = poDescriptorPool;
    tElementAllocInfo.descriptorSetCount = aoElementLayouts.size();
    tElementAllocInfo.pSetLayouts = aoElementLayouts.data();

    paoElementDescriptorSets.resize(paoElementUniformBuffers.size());
    if (!ppLogicalDevice->AllocateDescriptorSets(&tElementAllocInfo, paoElementDescriptorSets.data()))
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    uint uiIndex = 0;
    for (auto oObject : pScene->GetOverlay())
    {
        VkDescriptorBufferInfo tElementBufferInfo = {};
        tElementBufferInfo.buffer = paoElementUniformBuffers[uiIndex];
        tElementBufferInfo.offset = 0;
        tElementBufferInfo.range = sizeof(tOverlayElementObject);

        cTexture* pTexture = oObject.second->GetTexture();
        VkDescriptorImageInfo tElementImageInfo = {};
        tElementImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        tElementImageInfo.imageView = pTexture->GetView();
        tElementImageInfo.sampler = pTexture->GetSampler();

        std::array<VkWriteDescriptorSet, 2> atElementDescriptorWrites = {};

        atElementDescriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        atElementDescriptorWrites[0].dstSet = paoElementDescriptorSets[uiIndex];
        atElementDescriptorWrites[0].dstBinding = 0;
        atElementDescriptorWrites[0].dstArrayElement = 0;
        atElementDescriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        atElementDescriptorWrites[0].descriptorCount = 1;
        atElementDescriptorWrites[0].pImageInfo = &tElementImageInfo;

        atElementDescriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        atElementDescriptorWrites[1].dstSet = paoElementDescriptorSets[uiIndex];
        atElementDescriptorWrites[1].dstBinding = 1;
        atElementDescriptorWrites[1].dstArrayElement = 0;
        atElementDescriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        atElementDescriptorWrites[1].descriptorCount = 1;
        atElementDescriptorWrites[1].pBufferInfo = &tElementBufferInfo;

        ppLogicalDevice->UpdateDescriptorSets(atElementDescriptorWrites.size(), atElementDescriptorWrites.data(),
                                              0, nullptr);

        uiIndex++;
    }
}

void cOverlayUniformHandler::UpdateUniformBuffers(cScene* pScene)
{
    tOverlayUniformObject tObject = {};

    // If no scene is loaded, just use white
    tObject.color = glm::vec3(pScene == nullptr ? glm::vec3(1, 1, 1) : pScene->textColor);

    void* data;
    ppLogicalDevice->MapMemory(poBufferMemory, 0, sizeof(tObject), 0, &data);
    {
        memcpy(data, &tObject, sizeof(tObject));
    }
    ppLogicalDevice->UnmapMemory(poBufferMemory);

    uint uiIndex = 0;
    for (auto oElement : pScene->GetOverlay())
    {
        tOverlayElementObject tData = {};
        tData.tMatrix = oElement.second->GetMatrix(ppWindow);
        CopyToDeviceMemory(paoElementUniformBuffersMemory[uiIndex++], &tData, sizeof(tData));
    }
}

void cOverlayUniformHandler::CopyToDeviceMemory(VkDeviceMemory& oDeviceMemory, void* pData, uint uiDataSize)
{
    void* pMappedMemory;
    ppLogicalDevice->MapMemory(oDeviceMemory, 0, uiDataSize, 0, &pMappedMemory);
    {
        memcpy(pMappedMemory, pData, uiDataSize);
    }
    ppLogicalDevice->UnmapMemory(oDeviceMemory);
}

uint cOverlayUniformHandler::GetDescriptorSetLayoutCount(void)
{
    return 2;
}

VkDescriptorSetLayout* cOverlayUniformHandler::GetDescriptorSetLayouts(void)
{
    return paoDescriptorSetLayouts;
}

void cOverlayUniformHandler::CmdBindDescriptorSets(VkCommandBuffer& commandBuffer,
                                                   VkPipelineLayout& oPipelineLayout,
                                                   uint uiIndex)
{
    paoCurrentDescriptorSets[0] = poDescriptorSet;
    paoCurrentDescriptorSets[1] = paoElementDescriptorSets[uiIndex];
    vkCmdBindDescriptorSets(commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            oPipelineLayout, 0,
                            2, paoCurrentDescriptorSets,
                            0, nullptr);
}
