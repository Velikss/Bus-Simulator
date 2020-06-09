#pragma once

#include <pch.hpp>
#include <vulkan/scene/Scene.hpp>
#include <vulkan/util/GameManager.hpp>
#include <vulkan/scene/LightObject.hpp>

class cBlankScene : public cScene
{
    iGameManager* ppOverlayProvider;
public:
    cBlankScene(iGameManager* pOverlayProvider)
    {
        ppOverlayProvider = pOverlayProvider;
    }

protected:
    void Load(cTextureHandler* pTextureHandler, cLogicalDevice* pLogicalDevice, cAudioHandler* pAudioHandler) override
    {
        cScene::Load(pTextureHandler, pLogicalDevice, pAudioHandler);
    }

    void AfterLoad() override
    {
        ppOverlayProvider->ActivateOverlayWindow("MainMenu");
    }
};
