#pragma once

#include <pch.hpp>
#include <vulkan/scene/Scene.hpp>
#include <vulkan/module/overlay/OverlayProvider.hpp>
#include <vulkan/scene/LightObject.hpp>

class cBlankScene : public cScene
{
    iOverlayProvider* ppOverlayProvider;
public:
    cBlankScene(iOverlayProvider* pOverlayProvider)
    {
        ppOverlayProvider = pOverlayProvider;
    }

protected:
    void Load(cTextureHandler* pTextureHandler, cLogicalDevice* pLogicalDevice, cAudioHandler* pAudioHandler) override
    {
        cScene::Load(pTextureHandler, pLogicalDevice, pAudioHandler);
    }

    void AfterLoad()
    {
        ppOverlayProvider->ActivateOverlayWindow("MainMenu");
    }
};
