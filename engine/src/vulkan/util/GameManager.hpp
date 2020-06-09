#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/OverlayWindow.hpp>

class iGameManager
{
public:
    virtual cOverlayWindow* GetActiveOverlayWindow() = 0;
    virtual void ActivateOverlayWindow(const string& sName) = 0;
    virtual cOverlayWindow* GetOverlayByName(const string& sName) = 0;
    virtual void DeactivateOverlayWindow() = 0;
    virtual void SwitchScene(const string& sName) = 0;
    virtual std::map<string, cScene*>& GetScenes() = 0;
};
