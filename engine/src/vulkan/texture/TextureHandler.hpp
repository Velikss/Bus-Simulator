#pragma once

#include <pch.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/texture/Texture.hpp>
#include <vulkan/texture/TextureSampler.hpp>
#include <mutex>
#include <condition_variable>

// Class for loading and managing textures
class cTextureHandler
{
private:
    // Device where the textures and sampler are loaded
    cLogicalDevice* ppLogicalDevice = nullptr;

    // Texture sampler
    cTextureSampler* ppDefaultSampler = nullptr;
    cTextureSampler* ppSkyboxSampler = nullptr;

    // Map with all loaded textures
    // The key is the path to the texture
    std::map<string, cTexture*> pmpTextures;

    std::vector<std::thread*> papThreads;
    bool pbLoaderRunning = true;

    std::mutex ptLoadQueueMutex;
    std::condition_variable ptLoadQueueVariable;
    std::queue<cTexture*> papLoadQueue;

    std::mutex ptCopyQueueMutex;
    std::condition_variable ptCopyQueueVariable;
    std::queue<cTexture*> papCopyQueue;

    uint puiLoadingCount = 0;

public:
    cTextureHandler(cLogicalDevice* pLogicalDevice);
    ~cTextureHandler(void);

    // Load a texture from a file
    cTexture* LoadFromFile(const string& sFilePath, cTextureSampler* pSampler);
    cTexture* LoadFromFile(const string& sFilePath);

    void WaitForLoadComplete();

    // Returns the texture sampler
    cTextureSampler* GetDefaultSampler();
    cTextureSampler* GetSkyboxSampler();

private:
    void StartLoaderThread();
};

cTextureHandler::cTextureHandler(cLogicalDevice* pLogicalDevice)
{
    assert(pLogicalDevice != nullptr); // logical device must exist

    // Store the logical device
    ppLogicalDevice = pLogicalDevice;

    ppDefaultSampler = new cTextureSampler(pLogicalDevice,
                                           VK_FILTER_LINEAR,
                                           VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                           true);
    ppSkyboxSampler = new cTextureSampler(pLogicalDevice,
                                          VK_FILTER_NEAREST,
                                          VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                          false);

    for (uint uiIndex = 0; uiIndex < 4; uiIndex++)
    {
        papThreads.push_back(new std::thread(&cTextureHandler::StartLoaderThread, this));
    }
}

void cTextureHandler::StartLoaderThread()
{
    while (pbLoaderRunning)
    {
        std::unique_lock<std::mutex> tLoadLock(ptLoadQueueMutex);
        if (papLoadQueue.empty()) ptLoadQueueVariable.wait(tLoadLock);
        if (!pbLoaderRunning) return;

        cTexture* pTexture = papLoadQueue.front();
        papLoadQueue.pop();

        tLoadLock.unlock();

        pTexture->LoadIntoRAM();

        std::unique_lock<std::mutex> tCopyLock(ptCopyQueueMutex);
        papCopyQueue.push(pTexture);
        ptCopyQueueVariable.notify_one();
    }
}

void cTextureHandler::WaitForLoadComplete()
{
    while (puiLoadingCount > 0)
    {
        std::unique_lock<std::mutex> tCopyLock(ptCopyQueueMutex);
        if (papCopyQueue.empty()) ptCopyQueueVariable.wait(tCopyLock);
        if (!pbLoaderRunning) return;

        cTexture* pTexture = papCopyQueue.front();
        papCopyQueue.pop();

        tCopyLock.unlock();

        pTexture->CopyIntoGPU();
        pTexture->UnloadFromRAM();

        puiLoadingCount--;
    }
}

cTextureHandler::~cTextureHandler(void)
{
    // Set the loader running variable to false and notify all loader threads
    pbLoaderRunning = false;
    ptLoadQueueVariable.notify_all();

    // Join all the threads to terminate them
    for (std::thread* pThread : papThreads)
    {
        pThread->join();
    }

    // Delete all the textures
    for (auto&[sName, pTexture] : pmpTextures)
    {
        delete pTexture;
    }

    // Delete the texture samplers
    delete ppDefaultSampler;
    delete ppSkyboxSampler;
}

cTexture* cTextureHandler::LoadFromFile(const string& sFilePath, cTextureSampler* pSampler)
{
    cTexture* pTexture;

    // Try and find if this texture has already been loaded
    auto tResult = pmpTextures.find(sFilePath);
    if (tResult == pmpTextures.end())
    {
        // If not, create and load it
        pTexture = new cTexture(ppLogicalDevice, sFilePath, pSampler);

        std::unique_lock<std::mutex> tLoadLock(ptLoadQueueMutex);
        papLoadQueue.push(pTexture);
        puiLoadingCount++;
        ptLoadQueueVariable.notify_one();

        pmpTextures[sFilePath] = pTexture;

        return pTexture;
    }
    else
    {
        // If it's already loaded, just grab the loaded texture
        return tResult->second;
    }
}

cTexture* cTextureHandler::LoadFromFile(const string& sFilePath)
{
    return LoadFromFile(sFilePath, GetDefaultSampler());
}

cTextureSampler* cTextureHandler::GetDefaultSampler()
{
    return ppDefaultSampler;
}

cTextureSampler* cTextureHandler::GetSkyboxSampler()
{
    return ppSkyboxSampler;
}
