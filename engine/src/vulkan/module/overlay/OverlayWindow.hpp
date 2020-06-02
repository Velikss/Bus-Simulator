#pragma once

#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>
#include <vulkan/module/overlay/UIManager.hpp>
#include <vulkan/module/overlay/element/UIElement.hpp>
#include <vulkan/module/overlay/FocusHandler.hpp>

class cOverlayWindow : public iInputHandler, public iTickTask, public iFocusHandler
{
public:
    std::map<string, cUIElement*> pmpOverlay;

protected:
    std::map<string, cTexture*> pmpTextures;

    cUIManager* ppUIManager = nullptr;

    bool pbQuit = false;
    cFocussable* ppFocussedElement = nullptr;

public:
    virtual ~cOverlayWindow();

public:
    void Construct(cTextureHandler* pTextureHandler,
                   cLogicalDevice* pLogicalDevice,
                   iCommandBufferHolder* pCommandBufferHolder,
                   iCommandRecorderProvider* pCommandRecorder);

    virtual bool ShouldHandleInput() = 0;

    cUIManager* GetUIManager();

    bool ShouldQuit();
    void HandleMouse(uint uiDeltaX, uint uiDeltaY) override;
    void HandleKey(uint uiKeyCode, uint uiAction) override;
    void HandleScroll(double dOffsetX, double dOffsetY) override;
    void HandleCharacter(char cCharacter) override;
    void HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction) override;

    void SetFocussedElement(cFocussable* pUIElement) override;
    cFocussable* GetFocussedElement() override;

protected:
    void Quit();

    virtual void LoadTextures(cTextureHandler* pTextureHandler) = 0;
    virtual void ConstructElements() = 0;
};

cOverlayWindow::~cOverlayWindow()
{
    delete ppUIManager;

    for (auto oTexture : pmpTextures)
    {
        delete oTexture.second;
    }
}

void cOverlayWindow::Construct(cTextureHandler* pTextureHandler,
                               cLogicalDevice* pLogicalDevice,
                               iCommandBufferHolder* pCommandBufferHolder,
                               iCommandRecorderProvider* pCommandRecorder)
{
    ppUIManager = new cUIManager(pLogicalDevice, pCommandBufferHolder, pCommandRecorder);

    LoadTextures(pTextureHandler);
    ConstructElements();

    for (auto&[sName, pElement] : pmpOverlay)
    {
        pElement->OnPreLoad();
    }

    for (auto&[sName, pElement] : pmpOverlay)
    {
        pElement->SetParent(ppUIManager);
        pElement->SetFocusHandler(this);
        ppUIManager->patElements.emplace_back(pElement);
    }

    ppUIManager->AllocateElementsMemory();

    for (auto oTexture : pmpTextures)
    {
        assert(oTexture.second != nullptr);
    }
}

void cOverlayWindow::Quit()
{
    pbQuit = true;
}

bool cOverlayWindow::ShouldQuit()
{
    return pbQuit;
}

cUIManager* cOverlayWindow::GetUIManager()
{
    return ppUIManager;
}

void cOverlayWindow::SetFocussedElement(cFocussable* pUIElement)
{
    ppFocussedElement = pUIElement;
}

cFocussable* cOverlayWindow::GetFocussedElement()
{
    return ppFocussedElement;
}

void cOverlayWindow::HandleMouse(uint uiDeltaX, uint uiDeltaY)
{
    for (auto&[sName, pElement] : pmpOverlay)
    {
        iInputHandler* pInputHandler = dynamic_cast<iInputHandler*>(pElement);
        if (pInputHandler != nullptr)
        {
            pInputHandler->HandleMouse(uiDeltaX, uiDeltaY);
        }
    }
}

void cOverlayWindow::HandleKey(uint uiKeyCode, uint uiAction)
{
    for (auto&[sName, pElement] : pmpOverlay)
    {
        iInputHandler* pInputHandler = dynamic_cast<iInputHandler*>(pElement);
        if (pInputHandler != nullptr)
        {
            pInputHandler->HandleKey(uiKeyCode, uiAction);
        }
    }
}

void cOverlayWindow::HandleScroll(double dOffsetX, double dOffsetY)
{
    for (auto&[sName, pElement] : pmpOverlay)
    {
        iInputHandler* pInputHandler = dynamic_cast<iInputHandler*>(pElement);
        if (pInputHandler != nullptr)
        {
            pInputHandler->HandleScroll(dOffsetX, dOffsetY);
        }
    }
}

void cOverlayWindow::HandleCharacter(char cCharacter)
{
    for (auto&[sName, pElement] : pmpOverlay)
    {
        iInputHandler* pInputHandler = dynamic_cast<iInputHandler*>(pElement);
        if (pInputHandler != nullptr)
        {
            pInputHandler->HandleCharacter(cCharacter);
        }
    }
}

void cOverlayWindow::HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction)
{
    for (auto&[sName, pElement] : pmpOverlay)
    {
        iInputHandler* pInputHandler = dynamic_cast<iInputHandler*>(pElement);
        if (pInputHandler != nullptr)
        {
            pInputHandler->HandleMouseButton(uiButton, dXPos, dYPos, 0);
        }
    }
}
