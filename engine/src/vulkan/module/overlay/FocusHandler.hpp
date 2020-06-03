#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/element/Focussable.hpp>

class iFocusHandler
{
public:
    virtual void SetFocussedElement(cFocussable* pUIElement) = 0;
    virtual cFocussable* GetFocussedElement() = 0;
};
