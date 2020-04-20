#pragma once

#include <pch.hpp>
#include <fstream>
#include <vulkan/vulkan.h>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/SwapChain.hpp>
#include <vulkan/RenderPass.hpp>

class cGraphicsPipeline
{
private:
    cLogicalDevice* ppLogicalDevice;
    VkPipelineLayout poPipelineLayout;

    VkPipeline poGraphicsPipeline;

public:
    cGraphicsPipeline(cSwapChain* pSwapChain, cLogicalDevice* pLogicalDevice, cRenderPass* pRenderPass);
    ~cGraphicsPipeline(void);

private:
    VkShaderModule CreateShaderModule(const std::vector<char>& asCode, VkDevice& oDevice);
    std::vector<char> ReadFile(const std::string& filename);
};

cGraphicsPipeline::cGraphicsPipeline(cSwapChain* pSwapChain, cLogicalDevice* pLogicalDevice, cRenderPass* pRenderPass)
{
    ppLogicalDevice = pLogicalDevice;
    VkDevice& oDevice = pLogicalDevice->GetDevice();

    std::vector<char> acVertShaderCode = ReadFile("shaders/vert.spv");
    std::vector<char> acFragShaderCode = ReadFile("shaders/frag.spv");

    VkShaderModule oVertShaderModule = CreateShaderModule(acVertShaderCode, oDevice);
    VkShaderModule oFragShaderModule = CreateShaderModule(acFragShaderCode, oDevice);

    VkPipelineShaderStageCreateInfo tVertShaderStageInfo = {};
    tVertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    tVertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    tVertShaderStageInfo.module = oVertShaderModule;
    tVertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo tFragShaderStageInfo = {};
    tFragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    tFragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    tFragShaderStageInfo.module = oFragShaderModule;
    tFragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo atShaderStages[] = {tVertShaderStageInfo, tFragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo tVertexInputInfo = {};
    tVertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    tVertexInputInfo.vertexBindingDescriptionCount = 0;
    tVertexInputInfo.pVertexBindingDescriptions = NULL;
    tVertexInputInfo.vertexAttributeDescriptionCount = 0;
    tVertexInputInfo.pVertexAttributeDescriptions = NULL;

    VkPipelineInputAssemblyStateCreateInfo tInputAssemblyInfo = {};
    tInputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    tInputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    tInputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport tViewport = {};
    tViewport.x = 0.0f;
    tViewport.y = 0.0f;
    tViewport.width = (float) pSwapChain->ptSwapChainExtent.width;
    tViewport.height = (float) pSwapChain->ptSwapChainExtent.height;
    tViewport.minDepth = 0.0f;
    tViewport.maxDepth = 1.0f;

    VkRect2D tScissors = {};
    tScissors.offset = {0, 0};
    tScissors.extent = pSwapChain->ptSwapChainExtent;

    VkPipelineViewportStateCreateInfo tViewportState{};
    tViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    tViewportState.viewportCount = 1;
    tViewportState.pViewports = &tViewport;
    tViewportState.scissorCount = 1;
    tViewportState.pScissors = &tScissors;

    VkPipelineRasterizationStateCreateInfo tRasterizer = {};
    tRasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    tRasterizer.depthClampEnable = VK_FALSE;
    tRasterizer.rasterizerDiscardEnable = VK_FALSE;
    tRasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    tRasterizer.lineWidth = 1.0f;
    tRasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    tRasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    tRasterizer.depthBiasEnable = VK_FALSE;
    tRasterizer.depthBiasConstantFactor = 0.0f;
    tRasterizer.depthBiasClamp = 0.0f;
    tRasterizer.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo tMultisampling = {};
    tMultisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    tMultisampling.sampleShadingEnable = VK_FALSE;
    tMultisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    tMultisampling.minSampleShading = 1.0f;
    tMultisampling.pSampleMask = NULL;
    tMultisampling.alphaToCoverageEnable = VK_FALSE;
    tMultisampling.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState tColorBlendAttachment = {};
    tColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                           VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    tColorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo tColorBlending = {};
    tColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    tColorBlending.logicOpEnable = VK_FALSE;
    tColorBlending.attachmentCount = 1;
    tColorBlending.pAttachments = &tColorBlendAttachment;

    VkDynamicState aoDynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_LINE_WIDTH
    };

    VkPipelineDynamicStateCreateInfo tDynamicState = {};
    tDynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    tDynamicState.dynamicStateCount = 2;
    tDynamicState.pDynamicStates = aoDynamicStates;

    VkPipelineLayoutCreateInfo tPipelineLayoutInfo = {};
    tPipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    tPipelineLayoutInfo.setLayoutCount = 0;
    tPipelineLayoutInfo.pSetLayouts = NULL;
    tPipelineLayoutInfo.pushConstantRangeCount = 0;
    tPipelineLayoutInfo.pPushConstantRanges = NULL;

    if (vkCreatePipelineLayout(oDevice, &tPipelineLayoutInfo, NULL, &poPipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = atShaderStages;

    pipelineInfo.pVertexInputState = &tVertexInputInfo;
    pipelineInfo.pInputAssemblyState = &tInputAssemblyInfo;
    pipelineInfo.pViewportState = &tViewportState;
    pipelineInfo.pRasterizationState = &tRasterizer;
    pipelineInfo.pMultisampleState = &tMultisampling;
    pipelineInfo.pDepthStencilState = NULL;
    pipelineInfo.pColorBlendState = &tColorBlending;
    pipelineInfo.pDynamicState = &tDynamicState;

    pipelineInfo.layout = poPipelineLayout;

    pipelineInfo.renderPass = pRenderPass->poRenderPass;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(oDevice, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &poGraphicsPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(oDevice, oVertShaderModule, NULL);
    vkDestroyShaderModule(oDevice, oFragShaderModule, NULL);
}

cGraphicsPipeline::~cGraphicsPipeline()
{
    VkDevice& oDevice = ppLogicalDevice->GetDevice();
    vkDestroyPipeline(oDevice, poGraphicsPipeline, NULL);
    vkDestroyPipelineLayout(oDevice, poPipelineLayout, NULL);
}

VkShaderModule cGraphicsPipeline::CreateShaderModule(const std::vector<char>& asCode, VkDevice& oDevice)
{
    // Struct with information for creating the shader module
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    // Put the code into the struct
    createInfo.codeSize = asCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(asCode.data());

    // Create the shader module
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(oDevice, &createInfo, NULL, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

std::vector<char> cGraphicsPipeline::ReadFile(const string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}
