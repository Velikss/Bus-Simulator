#pragma once

#include <pch.hpp>
#include <fstream>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/SwapChain.hpp>
#include <vulkan/RenderPass.hpp>
#include <vulkan/Vertex.hpp>

class cGraphicsPipeline
{
private:
    cLogicalDevice* ppLogicalDevice;
    VkPipelineLayout poPipelineLayout;

public:
    VkPipeline poGraphicsPipeline;

    cGraphicsPipeline(cSwapChain* pSwapChain, cLogicalDevice* pLogicalDevice, cRenderPass* pRenderPass);
    ~cGraphicsPipeline(void);

private:
    VkShaderModule CreateShaderModule(const std::vector<char>& asCode, cLogicalDevice* pLogicalDevice);
    std::vector<char> ReadFile(const std::string& sFilename);
};

cGraphicsPipeline::cGraphicsPipeline(cSwapChain* pSwapChain, cLogicalDevice* pLogicalDevice, cRenderPass* pRenderPass)
{
    ppLogicalDevice = pLogicalDevice;

    // Read the shader files
    std::vector<char> acVertShaderCode = ReadFile("shaders/vert.spv");
    std::vector<char> acFragShaderCode = ReadFile("shaders/frag.spv");

    // Load the shader code into modules
    VkShaderModule oVertShaderModule = CreateShaderModule(acVertShaderCode, pLogicalDevice);
    VkShaderModule oFragShaderModule = CreateShaderModule(acFragShaderCode, pLogicalDevice);

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
    auto tBindingDescription = Vertex::GetBindingDescription();
    auto atAttributeDescriptions = Vertex::GetAttributeDescriptions();

    // Struct with information about the data we want to pass into the vertex shader
    VkPipelineVertexInputStateCreateInfo tVertexInputInfo = {};
    tVertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    tVertexInputInfo.vertexBindingDescriptionCount = 1;
    tVertexInputInfo.pVertexBindingDescriptions = &tBindingDescription;
    tVertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint>(atAttributeDescriptions.size());
    tVertexInputInfo.pVertexAttributeDescriptions = atAttributeDescriptions.data();

    // Struct with information about the kind of geometry we want to draw
    VkPipelineInputAssemblyStateCreateInfo tInputAssemblyInfo = {};
    tInputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    tInputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    tInputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

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

    // Struct with information for the rasterizer
    VkPipelineRasterizationStateCreateInfo tRasterizer = {};
    tRasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

    // Allows pixels beyond the near&far planes to be clamped to them.
    // Requires enabling this feature first
    tRasterizer.depthClampEnable = VK_FALSE;

    // If set to true, geometry will not pass trough the rasterizer stage
    tRasterizer.rasterizerDiscardEnable = VK_FALSE;

    // Determines how fragments are generated, options are FILL, LINE or POINT
    tRasterizer.polygonMode = VK_POLYGON_MODE_FILL;

    // Width of the lines (in fragments)
    // Values more than 1.0 require enabling this feature first
    tRasterizer.lineWidth = 1.0f;

    // Determines what type of culling to use
    tRasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    tRasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    // Allows the rasterizer to alter the depth values by adding a constant value or using a slope
    tRasterizer.depthBiasEnable = VK_FALSE;
    tRasterizer.depthBiasConstantFactor = 0.0f;
    tRasterizer.depthBiasClamp = 0.0f;
    tRasterizer.depthBiasSlopeFactor = 0.0f;

    // Struct with information to configure multisampling
    // Requires enabling this feature first
    VkPipelineMultisampleStateCreateInfo tMultisampling = {};
    tMultisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    tMultisampling.sampleShadingEnable = VK_FALSE;
    tMultisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    tMultisampling.minSampleShading = 1.0f;
    tMultisampling.pSampleMask = NULL;
    tMultisampling.alphaToCoverageEnable = VK_FALSE;
    tMultisampling.alphaToOneEnable = VK_FALSE;

    // Allows you to blend the new color values with values already in the framebuffer
    // We will just be ignoring this
    VkPipelineColorBlendAttachmentState tColorBlendAttachment = {};
    tColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                           VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    tColorBlendAttachment.blendEnable = VK_FALSE;
    VkPipelineColorBlendStateCreateInfo tColorBlending = {};
    tColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    tColorBlending.logicOpEnable = VK_FALSE;
    tColorBlending.attachmentCount = 1;
    tColorBlending.pAttachments = &tColorBlendAttachment;

    // Array of states you want to be able to change dynamically
    VkDynamicState aoDynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT
    };

    // Struct with information on the states you want to change dynamically
    VkPipelineDynamicStateCreateInfo tDynamicState = {};
    tDynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    tDynamicState.dynamicStateCount = 1;
    tDynamicState.pDynamicStates = aoDynamicStates;

    // Struct with information about the pipeline layout
    VkPipelineLayoutCreateInfo tPipelineLayoutInfo = {};
    tPipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    tPipelineLayoutInfo.setLayoutCount = 0;
    tPipelineLayoutInfo.pSetLayouts = NULL;
    tPipelineLayoutInfo.pushConstantRangeCount = 0;
    tPipelineLayoutInfo.pPushConstantRanges = NULL;

    // Create the pipeline layout
    if (!pLogicalDevice->CreatePipelineLayout(&tPipelineLayoutInfo, NULL, &poPipelineLayout))
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    // Struct with information for creating the graphics pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    // Set the shader stages we defined earlier
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = atShaderStages;

    // Set the configuration for all the fixed-function stages we defined earlier
    pipelineInfo.pVertexInputState = &tVertexInputInfo;
    pipelineInfo.pInputAssemblyState = &tInputAssemblyInfo;
    pipelineInfo.pViewportState = &tViewportState;
    pipelineInfo.pRasterizationState = &tRasterizer;
    pipelineInfo.pMultisampleState = &tMultisampling;
    pipelineInfo.pDepthStencilState = NULL;
    pipelineInfo.pColorBlendState = &tColorBlending;
    pipelineInfo.pDynamicState = NULL; // TODO: Enable dynamic states

    // Set the pipeline layout we created
    pipelineInfo.layout = poPipelineLayout;

    // Set the render pass and the subpass index
    pipelineInfo.renderPass = pRenderPass->poRenderPass;
    pipelineInfo.subpass = 0;

    // To change any of the above configurations during runtime (except the ones defined in the dynamic state)
    // the entire pipeline has to be re-created, and the old pipeline has to be passed in here
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    // Create the graphics pipeline
    if (!ppLogicalDevice->CreateGraphicsPipeline(1, &pipelineInfo, &poGraphicsPipeline))
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    // After the pipeline has been created, the shader modules can be destroyed
    pLogicalDevice->DestroyShaderModule(oVertShaderModule, NULL);
    pLogicalDevice->DestroyShaderModule(oFragShaderModule, NULL);
}

cGraphicsPipeline::~cGraphicsPipeline()
{
    ppLogicalDevice->DestroyPipeline(poGraphicsPipeline, NULL);
    ppLogicalDevice->DestroyPipelineLayout(poPipelineLayout, NULL);
}

VkShaderModule cGraphicsPipeline::CreateShaderModule(const std::vector<char>& asCode, cLogicalDevice* pLogicalDevice)
{
    // Struct with information for creating the shader module
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    // Put the code into the struct
    createInfo.codeSize = asCode.size();
    createInfo.pCode = reinterpret_cast<const uint*>(asCode.data());

    // Create the shader module
    VkShaderModule shaderModule;
    if (!pLogicalDevice->CreateShaderModule(&createInfo, NULL, &shaderModule))
    {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

std::vector<char> cGraphicsPipeline::ReadFile(const string& sFilename)
{
    std::ifstream oFileStream(sFilename, std::ios::ate | std::ios::binary);

    if (!oFileStream.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t uiFileSize = (size_t) oFileStream.tellg();
    std::vector<char> acBuffer(uiFileSize);

    oFileStream.seekg(0);
    oFileStream.read(acBuffer.data(), uiFileSize);

    oFileStream.close();

    return acBuffer;
}
