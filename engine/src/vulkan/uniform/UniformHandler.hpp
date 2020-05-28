#pragma once

#include <pch.hpp>
#include <vulkan/texture/TextureHandler.hpp>
#include <vulkan/scene/Scene.hpp>

class iUniformHandler
{
public:
    virtual ~iUniformHandler();

    virtual void SetupUniformBuffers(cTextureHandler* pTextureHandler, cScene* pScene) = 0;
    virtual void UpdateUniformBuffers(cScene* pScene);

    virtual uint GetDescriptorSetLayoutCount(void) = 0;
    virtual VkDescriptorSetLayout* GetDescriptorSetLayouts(void) = 0;

    virtual void CmdBindDescriptorSets(VkCommandBuffer& commandBuffer,
                                       VkPipelineLayout& oPipelineLayout,
                                       uint uiIndex) = 0;
};

iUniformHandler::~iUniformHandler()
{

}

void iUniformHandler::UpdateUniformBuffers(cScene* pScene)
{
    ENGINE_WARN("Aaaaaaaaaaaah");
}
