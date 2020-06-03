#pragma once

#include <pch.hpp>
#include <vulkan/scene/Scene.hpp>
#include <vulkan/util/EngineLog.hpp>

class cSceneManager
{
private:
    cScene* ppActiveScene = nullptr;
    std::map<string, cScene*> pmpScenes;

    cLogicalDevice* ppLogicalDevice = nullptr;
    cTextureHandler* ppTextureHandler = nullptr;
    cAudioHandler* ppAudioHandler = nullptr;

public:
    cSceneManager(cLogicalDevice* ppLogicalDevice, cTextureHandler* ppTextureHandler, cAudioHandler* ppAudioHandler);
    void LoadScene(const string& sName);

    std::map<string, cScene*>& GetScenes();
    cScene* GetActiveScene();
    virtual ~cSceneManager();
};

cSceneManager::cSceneManager(cLogicalDevice* pLogicalDevice,
                             cTextureHandler* pTextureHandler,
                             cAudioHandler* pAudioHandler)
{
    assert(pLogicalDevice != nullptr);
    assert(pTextureHandler != nullptr);
    assert(pAudioHandler != nullptr);

    ppLogicalDevice = pLogicalDevice;
    ppTextureHandler = pTextureHandler;
    ppAudioHandler = pAudioHandler;
}

cSceneManager::~cSceneManager()
{
    for (auto&[sName, pScene] : pmpScenes)
    {
        delete pScene;
    }
}

void cSceneManager::LoadScene(const string& sName)
{
    auto tResult = pmpScenes.find(sName);
    assert(tResult != pmpScenes.end()); // scene must exist

    if (ppActiveScene != nullptr)
    {
        ENGINE_LOG("Unloading active scene...");
        ppActiveScene->Unload();
    }

    ENGINE_LOG("Loading new scene...");
    ppActiveScene = tResult->second;
    ppActiveScene->Load(ppTextureHandler, ppLogicalDevice, ppAudioHandler);
}

std::map<string, cScene*>& cSceneManager::GetScenes()
{
    return pmpScenes;
}

cScene* cSceneManager::GetActiveScene()
{
    return ppActiveScene;
}
