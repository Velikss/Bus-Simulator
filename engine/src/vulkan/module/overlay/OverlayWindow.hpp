#pragma once

#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>

class cOverlayWindow : public iInputHandler
{
private:
    bool pbQuit = false;

protected:
    std::map<string, cTexture*> pmpTextures;
    std::map<string, cStaticElement*> pmpOverlay;

public:
    virtual ~cOverlayWindow();

public:
    void Construct(cTextureHandler* pTextureHandler, cLogicalDevice* pLogicalDevice);

    std::map<string, cStaticElement*>& GetElements();

    virtual bool ShouldHandleInput() = 0;

    bool ShouldQuit();

protected:
    void Quit();

    virtual void LoadTextures(cTextureHandler* pTextureHandler) = 0;
    virtual void ConstructElements(cLogicalDevice* pLogicalDevice) = 0;
};

cOverlayWindow::~cOverlayWindow()
{
    for (auto oElement : pmpOverlay)
    {
        delete oElement.second;
    }

    for (auto oTexture : pmpTextures)
    {
        delete oTexture.second;
    }
}

void cOverlayWindow::Construct(cTextureHandler* pTextureHandler, cLogicalDevice* pLogicalDevice)
{
    LoadTextures(pTextureHandler);
    ConstructElements(pLogicalDevice);

    for (auto oElement : pmpOverlay)
    {
        assert(oElement.second != nullptr);
        oElement.second->OnLoadVertices();
        oElement.second->CopyToDevice();
    }

    for (auto oTexture : pmpTextures)
    {
        assert(oTexture.second != nullptr);
    }
}

std::map<string, cStaticElement*>& cOverlayWindow::GetElements()
{
    return pmpOverlay;
}

void cOverlayWindow::Quit()
{
    pbQuit = true;
}

bool cOverlayWindow::ShouldQuit()
{
    return pbQuit;
}
