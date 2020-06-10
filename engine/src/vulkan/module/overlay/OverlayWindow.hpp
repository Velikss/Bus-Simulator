#pragma once

#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>
#include <vulkan/module/overlay/UIManager.hpp>
#include <vulkan/module/overlay/element/UIElement.hpp>
#include <vulkan/module/overlay/FocusHandler.hpp>

class cOverlayWindow : public iInputHandler, public iTickTask, public iFocusHandler
{
public:
    std::vector<std::pair<string, cUIElement*>> pmpOverlay;

protected:
    std::map<string, cTexture*> pmpTextures;

    cUIManager* ppUIManager = nullptr;

    bool pbQuit = false;
    cFocussable* ppFocussedElement = nullptr;

    cUIElement* GetElement(const string& sName);
public:
    virtual ~cOverlayWindow();

public:
    void Construct(cTextureHandler* pTextureHandler,
                   cLogicalDevice* pLogicalDevice,
                   iCommandBufferHolder* pCommandBufferHolder,
                   iCommandRecorderProvider* pCommandRecorder);

    virtual bool ShouldHandleInput() = 0;
    virtual void OnOpen();
    virtual void OnClose();

    cUIManager* GetUIManager();

    bool ShouldQuit();
    void HandleMouse(double dDeltaX, double dDeltaY) override;
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

    for (auto&[sName, pElement] : pmpOverlay)
    {
        delete pElement;
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
    if (pbQuit)
    {
        pbQuit = false;
        return true;
    }
    return false;
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

void cOverlayWindow::HandleMouse(double dDeltaX, double dDeltaY)
{
    for (auto&[sName, pElement] : pmpOverlay)
    {
        iInputHandler* pInputHandler = dynamic_cast<iInputHandler*>(pElement);
        if (pInputHandler != nullptr)
        {
            pInputHandler->HandleMouse(dDeltaX, dDeltaY);
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
            pInputHandler->HandleMouseButton(uiButton, dXPos, dYPos, iAction);
        }
    }
}

cUIElement* cOverlayWindow::GetElement(const string& sName)
{
    auto oElem = std::find_if(pmpOverlay.begin(), pmpOverlay.end(),
                              [&](const std::pair<string, cUIElement*>& poElem)
                              { return poElem.first == sName; });
    if (oElem != pmpOverlay.end())
        return oElem->second;
    else
        return nullptr;
}

void cOverlayWindow::OnOpen()
{

}

void cOverlayWindow::OnClose()
{

}
