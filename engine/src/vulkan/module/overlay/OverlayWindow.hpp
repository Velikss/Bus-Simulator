#pragma once

#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>
#include <vulkan/module/overlay/UIManager.hpp>

class cOverlayWindow : public iInputHandler, public iTickTask
{
private:
    bool pbQuit = false;

protected:
    std::map<string, cTexture*> pmpTextures;

    cUIManager* ppUIManager = nullptr;

public:
    std::map<string, cUIElement*> pmpOverlay;

    virtual ~cOverlayWindow();

public:
    void Construct(cTextureHandler* pTextureHandler,
                   cLogicalDevice* pLogicalDevice,
                   iCommandBufferHolder* pCommandBufferHolder,
                   iCommandRecorderProvider* pCommandRecorder);

    virtual bool ShouldHandleInput() = 0;

    cUIManager* GetUIManager();

    bool ShouldQuit();

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
        pElement->SetParent(ppUIManager);
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
