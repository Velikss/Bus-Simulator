#pragma once

#include <pch.hpp>

class iInputHandler
{
public:
    virtual void HandleMouse(uint uiDeltaX, uint uiDeltaY);
    virtual void HandleKey(uint uiKeyCode, uint uiAction);
    virtual void HandleScroll(double dOffsetX, double dOffsetY);
    virtual void HandleCharacter(char cCharacter);
    virtual void HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction);
};

void iInputHandler::HandleMouse(uint uiDeltaX, uint uiDeltaY)
{

}

void iInputHandler::HandleKey(uint uiKeyCode, uint uiAction)
{

}

void iInputHandler::HandleScroll(double dOffsetX, double dOffsetY)
{

}

void iInputHandler::HandleCharacter(char cCharacter)
{

}

void iInputHandler::HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction)
{

}
