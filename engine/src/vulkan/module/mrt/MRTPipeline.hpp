#pragma once

#include <pch.hpp>
#include <fstream>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/swapchain/SwapChain.hpp>
#include <vulkan/geometry/Vertex.hpp>
#include <vulkan/pipeline/PipelineHelper.hpp>
#include <vulkan/pipeline/RenderPipeline.hpp>
#include <vulkan/module/overlay/text/Vertex2D.hpp>

class cMRTPipeline : public cRenderPipeline
{
public:
    cMRTPipeline(cSwapChain* pSwapChain,
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

cMRTPipeline::cMRTPipeline(cSwapChain* pSwapChain,
                           cLogicalDevice* pLogicalDevice,
                           cRenderPass* pRenderPass,
                           iUniformHandler* pUniformHandler,
                           std::vector<string>& aShaders)
{
    Init(pSwapChain, pLogicalDevice, pRenderPass, pUniformHandler, aShaders);
}

void cMRTPipeline::CreatePipelineLayout(cSwapChain* pSwapChain,
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

void cMRTPipeline::CreatePipeline(cSwapChain* pSwapChain,
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
    auto atBindingDescription = Vertex::GetBindingDescription();
    auto atAttributeDescriptions = Vertex::GetAttributeDescriptions();

    // Struct with information about the data we want to pass into the vertex shader
    VkPipelineVertexInputStateCreateInfo tVertexInputInfo = {};
    tVertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    tVertexInputInfo.vertexBindingDescriptionCount = 1;
    tVertexInputInfo.pVertexBindingDescriptions = &atBindingDescription;
    tVertexInputInfo.vertexAttributeDescriptionCount = atAttributeDescriptions.size();
    tVertexInputInfo.pVertexAttributeDescriptions = atAttributeDescriptions.data();

    // Struct with information about the kind of geometry we want to draw
    VkPipelineInputAssemblyStateCreateInfo tInputAssemblyInfo = {};
    tInputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    tInputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
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

    std::array<VkPipelineColorBlendAttachmentState, 4> atBlendAttachments;
    atBlendAttachments[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                           VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    atBlendAttachments[0].blendEnable = VK_FALSE;
    atBlendAttachments[0].srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    atBlendAttachments[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    atBlendAttachments[0].colorBlendOp = VK_BLEND_OP_ADD; // Optional
    atBlendAttachments[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    atBlendAttachments[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    atBlendAttachments[0].alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    atBlendAttachments[1].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                           VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    atBlendAttachments[1].blendEnable = VK_FALSE;
    atBlendAttachments[1].srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    atBlendAttachments[1].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    atBlendAttachments[1].colorBlendOp = VK_BLEND_OP_ADD; // Optional
    atBlendAttachments[1].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    atBlendAttachments[1].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    atBlendAttachments[1].alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    atBlendAttachments[2].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                           VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    atBlendAttachments[2].blendEnable = VK_FALSE;
    atBlendAttachments[2].srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    atBlendAttachments[2].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    atBlendAttachments[2].colorBlendOp = VK_BLEND_OP_ADD; // Optional
    atBlendAttachments[2].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    atBlendAttachments[2].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    atBlendAttachments[2].alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    atBlendAttachments[3].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                           VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    atBlendAttachments[3].blendEnable = VK_FALSE;
    atBlendAttachments[3].srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    atBlendAttachments[3].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    atBlendAttachments[3].colorBlendOp = VK_BLEND_OP_ADD; // Optional
    atBlendAttachments[3].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    atBlendAttachments[3].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    atBlendAttachments[3].alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo tColorBlending = {};
    tColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    tColorBlending.logicOpEnable = VK_FALSE;
    tColorBlending.attachmentCount = atBlendAttachments.size();
    tColorBlending.pAttachments = atBlendAttachments.data();

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
            cPipelineHelper::GetRasterizerCreateInfo(VK_FRONT_FACE_COUNTER_CLOCKWISE);
    VkPipelineDepthStencilStateCreateInfo tDepthStencil = cPipelineHelper::GetDepthStencilCreateInfo();

    // Set the configuration for all the fixed-function stages we defined earlier
    pipelineInfo.pVertexInputState = &tVertexInputInfo;
    pipelineInfo.pInputAssemblyState = &tInputAssemblyInfo;
    pipelineInfo.pViewportState = &tViewportState;
    pipelineInfo.pRasterizationState = &tRasterizer;
    pipelineInfo.pMultisampleState = &tMultisampling;
    pipelineInfo.pDepthStencilState = &tDepthStencil;
    pipelineInfo.pColorBlendState = &tColorBlending;
    pipelineInfo.pDynamicState = nullptr; // TODO: Enable dynamic states

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
