#pragma once

#include <pch.hpp>
#include <fstream>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/swapchain/SwapChain.hpp>
#include <vulkan/module/lighting/LightingRenderPass.hpp>
#include <vulkan/geometry/Vertex.hpp>
#include <vulkan/module/mrt/MRTUniformHandler.hpp>
#include <vulkan/pipeline/PipelineHelper.hpp>
#include <vulkan/module/overlay/text/Vertex2D.hpp>
#include <vulkan/module/overlay/OverlayRenderPass.hpp>

class cOverlayPipeline : public cRenderPipeline
{
public:
    cOverlayPipeline(cSwapChain* pSwapChain,
                     cLogicalDevice* pLogicalDevice,
                     cRenderPass* pRenderPass,
                     iUniformHandler* pUniformHandler,
                     std::vector<string>& aShaders);

protected:
    void CreatePipelineLayout(cSwapChain* pSwapChain,
                              cLogicalDevice* pLogicalDevice,
                              cRenderPass* pRenderPass,
                              iUniformHandler* pUniformHandler) override;
    void CreatePipeline(cSwapChain* pSwapChain,
                        cLogicalDevice* pLogicalDevice,
                        cRenderPass* pRenderPass,
                        iUniformHandler* pUniformHandler,
                        std::vector<string>& aShaders) override;
};

cOverlayPipeline::cOverlayPipeline(cSwapChain* pSwapChain,
                                   cLogicalDevice* pLogicalDevice,
                                   cRenderPass* pRenderPass,
                                   iUniformHandler* pUniformHandler,
                                   std::vector<string>& aShaders)
{
    Init(pSwapChain, pLogicalDevice, pRenderPass, pUniformHandler, aShaders);
}

void cOverlayPipeline::CreatePipelineLayout(cSwapChain* pSwapChain,
                                            cLogicalDevice* pLogicalDevice,
                                            cRenderPass* pRenderPass,
                                            iUniformHandler* pUniformHandler)
{
    // Struct with information about the pipeline layout
    VkPipelineLayoutCreateInfo tPipelineLayoutInfo = {};
    tPipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    tPipelineLayoutInfo.setLayoutCount = pUniformHandler->GetDescriptorSetLayoutCount();
    tPipelineLayoutInfo.pSetLayouts = pUniformHandler->GetDescriptorSetLayouts();
    tPipelineLayoutInfo.pushConstantRangeCount = 0;
    tPipelineLayoutInfo.pPushConstantRanges = nullptr;

    // Create the pipeline layout
    if (!pLogicalDevice->CreatePipelineLayout(&tPipelineLayoutInfo, nullptr, &poPipelineLayout))
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void cOverlayPipeline::CreatePipeline(cSwapChain* pSwapChain,
                                      cLogicalDevice* pLogicalDevice,
                                      cRenderPass* pRenderPass,
                                      iUniformHandler* pUniformHandler,
                                      std::vector<string>& aShaders)
{
    // Read the shader files
    std::vector<char> acVertShaderCode = cPipelineHelper::ReadFile(aShaders[0]);
    std::vector<char> acFragShaderCode = cPipelineHelper::ReadFile(aShaders[1]);

    // Load the shader code into modules
    VkShaderModule oVertShaderModule = cPipelineHelper::CreateShaderModule(acVertShaderCode, pLogicalDevice);
    VkShaderModule oFragShaderModule = cPipelineHelper::CreateShaderModule(acFragShaderCode, pLogicalDevice);

    // Struct with information about the vertex shader stage
    VkPipelineShaderStageCreateInfo tVertShaderStageInfo = {};
    tVertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    tVertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    tVertShaderStageInfo.module = oVertShaderModule;
    tVertShaderStageInfo.pName = "main";

    // Struct with information about the fragment shader stage
    VkPipelineShaderStageCreateInfo tFragShaderStageInfo = {};
    tFragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    tFragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    tFragShaderStageInfo.module = oFragShaderModule;
    tFragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo atShaderStages[] = {tVertShaderStageInfo, tFragShaderStageInfo};

    // Get the vertex binding description and attribute descriptions
    auto atBindingDescriptions = tVertex2D::GetBindingDescriptions();
    auto atAttributeDescriptions = tVertex2D::GetAttributeDescriptions();

    // Struct with information about the data we want to pass into the vertex shader
    VkPipelineVertexInputStateCreateInfo tVertexInputInfo = {};
    tVertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    tVertexInputInfo.vertexBindingDescriptionCount = (uint) atBindingDescriptions.size();
    tVertexInputInfo.pVertexBindingDescriptions = atBindingDescriptions.data();
    tVertexInputInfo.vertexAttributeDescriptionCount = (uint) atAttributeDescriptions.size();
    tVertexInputInfo.pVertexAttributeDescriptions = atAttributeDescriptions.data();

    // Struct with information about the kind of geometry we want to draw
    VkPipelineInputAssemblyStateCreateInfo tInputAssemblyInfo = {};
    tInputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    tInputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    tInputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    // Struct with information to configure multisampling
    // Requires enabling this feature first
    VkPipelineMultisampleStateCreateInfo tMultisampling = {};
    tMultisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    tMultisampling.sampleShadingEnable = VK_FALSE;
    tMultisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    tMultisampling.minSampleShading = 1.0f;
    tMultisampling.pSampleMask = nullptr;
    tMultisampling.alphaToCoverageEnable = VK_FALSE;
    tMultisampling.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState tColorBlendAttachment = {};
    tColorBlendAttachment.blendEnable = VK_TRUE;
    tColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                           VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    tColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    tColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    tColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    tColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    tColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    tColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_MAX;

    VkPipelineColorBlendStateCreateInfo tColorBlending = {};
    tColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    tColorBlending.logicOpEnable = VK_FALSE;
    tColorBlending.attachmentCount = 1;
    tColorBlending.pAttachments = &tColorBlendAttachment;

    // Struct with information for creating the graphics pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    // Set the shader stages we defined earlier
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = atShaderStages;

    // Struct with information about the viewport
    VkViewport tViewport = {};
    tViewport.x = 0.0f;
    tViewport.y = 0.0f;
    tViewport.width = (float) pSwapChain->ptSwapChainExtent.width;
    tViewport.height = (float) pSwapChain->ptSwapChainExtent.height;
    tViewport.minDepth = 0.0f;
    tViewport.maxDepth = 1.0f;

    // The scissors rectangle defines in which regions pixels will actually be stored
    // We just want it the same size as the frames themselves
    VkRect2D tScissors = {};
    tScissors.offset = {0, 0};
    tScissors.extent = pSwapChain->ptSwapChainExtent;

    // Struct containing the previously made scissors rectangle and viewport info
    VkPipelineViewportStateCreateInfo tViewportState{};
    tViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    tViewportState.viewportCount = 1;
    tViewportState.pViewports = &tViewport;
    tViewportState.scissorCount = 1;
    tViewportState.pScissors = &tScissors;

    VkPipelineRasterizationStateCreateInfo tRasterizer =
            cPipelineHelper::GetRasterizerCreateInfo(VK_FRONT_FACE_CLOCKWISE);

    // Set the configuration for all the fixed-function stages we defined earlier
    pipelineInfo.pVertexInputState = &tVertexInputInfo;
    pipelineInfo.pInputAssemblyState = &tInputAssemblyInfo;
    pipelineInfo.pViewportState = &tViewportState;
    pipelineInfo.pRasterizationState = &tRasterizer;
    pipelineInfo.pMultisampleState = &tMultisampling;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pColorBlendState = &tColorBlending;
    pipelineInfo.pDynamicState = nullptr;

    // Set the pipeline layout we created
    pipelineInfo.layout = poPipelineLayout;

    // Set the render pass and the subpass index
    pipelineInfo.renderPass = pRenderPass->GetRenderPass();
    pipelineInfo.subpass = 0;

    // To change any of the above configurations during runtime (except the ones defined in the dynamic state)
    // the entire pipeline has to be re-created, and the old pipeline has to be passed in here
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    // Create the graphics pipeline
    if (!ppLogicalDevice->CreateGraphicsPipeline(1, &pipelineInfo, &poPipeline))
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    // After the pipeline has been created, the shader modules can be destroyed
    pLogicalDevice->DestroyShaderModule(oVertShaderModule, nullptr);
    pLogicalDevice->DestroyShaderModule(oFragShaderModule, nullptr);
}
