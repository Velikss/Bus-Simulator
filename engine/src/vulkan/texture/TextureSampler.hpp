#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/LogicalDevice.hpp>

class cTextureSampler
{
private:
    cLogicalDevice* ppLogicalDevice;

    VkSampler poTextureSampler = VK_NULL_HANDLE;

public:
    cTextureSampler(cLogicalDevice* pLogicalDevice,
                    VkFilter eFilter,
                    VkSamplerAddressMode eAddressMode,
                    bool bAnisotropy);
    ~cTextureSampler();

    VkSampler& GetSampler();
};

cTextureSampler::cTextureSampler(cLogicalDevice* pLogicalDevice,
                                 VkFilter eFilter,
                                 VkSamplerAddressMode eAddressMode,
                                 bool bAnisotropy)
{
    ppLogicalDevice = pLogicalDevice;

    VkSamplerCreateInfo tSamplerInfo = {};
    tSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

    // Filter mode to use to interpolate texels that are magnified or minified
    tSamplerInfo.magFilter = eFilter;
    tSamplerInfo.minFilter = eFilter;

    // The addressing mode to use per axis
    tSamplerInfo.addressModeU = eAddressMode;
    tSamplerInfo.addressModeV = eAddressMode;
    tSamplerInfo.addressModeW = eAddressMode;

    // Configure anisotropic filtering
    tSamplerInfo.anisotropyEnable = bAnisotropy;
    tSamplerInfo.maxAnisotropy = bAnisotropy ? cPhysicalDevice::GetInstance()->GetMaxAnisotropy() : 1;

    // When sampling outside the image, what color to use
    // irrelevant when using REPEAT addressing mode
    tSamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

    // When true,  UV coordinates range from 0 to textureWidth and 0 to textureHeight
    // When false, UV coordinates range from 0 to 1
    tSamplerInfo.unnormalizedCoordinates = VK_FALSE;

    // Allows for comparing texel values to a value and using the result of the comparison
    tSamplerInfo.compareEnable = VK_FALSE;
    tSamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    // Configure mipmapping. Currently disabled
    tSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    tSamplerInfo.mipLodBias = 0.0f;
    tSamplerInfo.minLod = 0.0f;
    tSamplerInfo.maxLod = 0.0f;

    if (!ppLogicalDevice->CreateSampler(&tSamplerInfo, nullptr, &poTextureSampler))
    {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

cTextureSampler::~cTextureSampler()
{
    ppLogicalDevice->DestroySampler(poTextureSampler, nullptr);
}

VkSampler& cTextureSampler::GetSampler()
{
    return poTextureSampler;
}
