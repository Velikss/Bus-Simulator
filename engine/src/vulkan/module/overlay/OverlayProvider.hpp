#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/OverlayWindow.hpp>

class iOverlayProvider
{
public:
    virtual cOverlayWindow* GetActiveOverlayWindow() = 0;
    virtual void ActivateOverlayWindow(const string& sName) = 0;
    virtual void DeactivateOverlayWindow() = 0;
};