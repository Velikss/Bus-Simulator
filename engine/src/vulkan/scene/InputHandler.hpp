#pragma once

#include <pch.hpp>

class iInputHandler
{
public:
    virtual void HandleMouse(uint uiDeltaX, uint uiDeltaY) = 0;
    virtual void HandleKey(uint uiKeyCode, uint uiAction) = 0;
    virtual void HandleScroll(double dOffsetX, double dOffsetY) = 0;
};
