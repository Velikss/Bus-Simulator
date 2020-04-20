#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/VulkanInstance.hpp>
#include "Window.hpp"

class cSurface
{
private:
    VkSurfaceKHR poSurface;

public:
    cSurface(cVulkanInstance* pVulkanInstance, cWindow* pWindow);

    void GetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice& oPhysicalDevice,
                                            uint uiQueueFamilyIndex,
                                            VkBool32* pSupported);

    VkSurfaceKHR& GetSurface(void);
};

cSurface::cSurface(cVulkanInstance* pVulkanInstance, cWindow* pWindow)
{
    pVulkanInstance->CreateWindowSurface(pWindow, NULL, &poSurface);
}

// TODO: This doesn't belong here, needs to be in PhysicalDevice
void cSurface::GetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice& oPhysicalDevice,
                                                  uint uiQueueFamilyIndex,
                                                  VkBool32* pSupported)
{
    vkGetPhysicalDeviceSurfaceSupportKHR(oPhysicalDevice, uiQueueFamilyIndex, poSurface, pSupported);
}

VkSurfaceKHR& cSurface::GetSurface(void)
{
    return poSurface;
}
