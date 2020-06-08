#pragma once
#include <pch.hpp>
#include <vulkan/module/overlay/OverlayWindow.hpp>
#include <vulkan/texture/TextureHandler.hpp>
#include <vulkan/util/GameManager.hpp>

class cInGame : public cOverlayWindow
{
private:
    iGameManager* ppOverlayProvider;

public:
    cInGame(iGameManager* pOverlayProvider) : ppOverlayProvider(pOverlayProvider)
    {}

protected:
    void LoadTextures(cTextureHandler* pTextureHandler) override
    {
    }

    void ConstructElements() override
    {

    }

    void Tick() override
    {
    }

    bool ShouldHandleInput() override
    {
        return true;
    }
};