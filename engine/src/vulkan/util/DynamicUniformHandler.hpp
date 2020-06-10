#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/element/UIElement.hpp>
#include <vulkan/uniform/UniformHandler.hpp>

class iDynamicUniformHandler : public iUniformHandler
{
public:
    virtual void NewDescriptorSet(cUIElement* pElement) = 0;
};
