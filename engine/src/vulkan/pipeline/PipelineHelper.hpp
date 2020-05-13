#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/LogicalDevice.hpp>

class cPipelineHelper
{
public:
    static VkShaderModule CreateShaderModule(const std::vector<char>& asCode, cLogicalDevice* pLogicalDevice);
    static std::vector<char> ReadFile(const std::string& sFilename);

    static VkPipelineRasterizationStateCreateInfo GetRasterizerCreateInfo(VkFrontFace eFrontFace);
    static VkPipelineDepthStencilStateCreateInfo GetDepthStencilCreateInfo();
};

VkShaderModule cPipelineHelper::CreateShaderModule(const std::vector<char>& asCode, cLogicalDevice* pLogicalDevice)
{
    // Struct with information for creating the shader module
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    // Put the code into the struct
    createInfo.codeSize = asCode.size();
    createInfo.pCode = reinterpret_cast<const uint*>(asCode.data());

    // Create the shader module
    VkShaderModule shaderModule;
    if (!pLogicalDevice->CreateShaderModule(&createInfo, nullptr, &shaderModule))
    {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

std::vector<char> cPipelineHelper::ReadFile(const string& sFilename)
{
    std::ifstream oFileStream(sFilename, std::ios::ate | std::ios::binary);

    if (!oFileStream.is_open())
    {
        throw std::runtime_error(cFormatter() << "failed to open shader file '" << sFilename << "'");
    }

    size_t uiFileSize = (size_t) oFileStream.tellg();
    std::vector<char> acBuffer(uiFileSize);

    oFileStream.seekg(0);
    oFileStream.read(acBuffer.data(), uiFileSize);

    oFileStream.close();

    ENGINE_LOG("Loaded shader file " << sFilename);

    return acBuffer;
}

VkPipelineRasterizationStateCreateInfo cPipelineHelper::GetRasterizerCreateInfo(VkFrontFace eFrontFace)
{
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
    tRasterizer.frontFace = eFrontFace;

    // Allows the rasterizer to alter the depth values by adding a constant value or using a slope
    tRasterizer.depthBiasEnable = VK_FALSE;
    tRasterizer.depthBiasConstantFactor = 0.0f;
    tRasterizer.depthBiasClamp = 0.0f;
    tRasterizer.depthBiasSlopeFactor = 0.0f;

    return tRasterizer;
}

VkPipelineDepthStencilStateCreateInfo cPipelineHelper::GetDepthStencilCreateInfo()
{
    // Struct with information about the depth stencil
    VkPipelineDepthStencilStateCreateInfo tDepthStencil = {};
    tDepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    // Enable depth testing
    tDepthStencil.depthTestEnable = VK_TRUE;
    tDepthStencil.depthWriteEnable = VK_TRUE;

    // We want to stick to the convention lower depth = closer, so new frags should be less
    tDepthStencil.depthCompareOp = VK_COMPARE_OP_LESS;

    // We don't have any special depth bounds
    tDepthStencil.depthBoundsTestEnable = VK_FALSE;

    // We don't want to use stencil buffer operations
    tDepthStencil.stencilTestEnable = VK_FALSE;

    return tDepthStencil;
}
