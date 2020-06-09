#pragma once

#include <pch.hpp>
#include <mutex>
#include <condition_variable>
#include <atomic>

template<class T>
class cAsyncLoader
{
private:
    std::vector<std::thread*> papThreads;
    bool pbLoaderRunning = true;

    std::mutex ptLoadQueueMutex;
    std::condition_variable ptLoadQueueVariable;
    std::queue<T*> papLoadQueue;

    std::mutex ptLoadDoneMutex;
    std::condition_variable ptLoadDoneVariable;

    std::atomic_uint puiLoadingCount = 0;

public:
    cAsyncLoader(uint uiThreadCount);
    virtual ~cAsyncLoader();

    void Load(T* pObject);
    void WaitForLoadComplete();

protected:
    virtual void LoadCallback(T* pObject) = 0;

private:
    void StartLoaderThread();
};

template<class T>
cAsyncLoader<T>::cAsyncLoader(uint uiThreadCount)
{
    for (uint uiIndex = 0; uiIndex < uiThreadCount; uiIndex++)
    {
        papThreads.push_back(new std::thread(&cAsyncLoader::StartLoaderThread, this));
    }
}

template<class T>
void cAsyncLoader<T>::StartLoaderThread()
{
    while (pbLoaderRunning)
    {
        std::unique_lock<std::mutex> tLoadLock(ptLoadQueueMutex);
        if (papLoadQueue.empty()) ptLoadQueueVariable.wait(tLoadLock);
        if (!pbLoaderRunning) return;

        T* pObject = papLoadQueue.front();
        papLoadQueue.pop();

        tLoadLock.unlock();

        LoadCallback(pObject);

        puiLoadingCount--;
        if (puiLoadingCount == 0)
        {
            ptLoadDoneVariable.notify_all();
        }
    }
}

template<class T>
void cAsyncLoader<T>::Load(T* pObject)
{
    ptLoadQueueMutex.lock();
    papLoadQueue.push(pObject);
    puiLoadingCount++;
    ptLoadQueueMutex.unlock();
    ptLoadQueueVariable.notify_one();
}

template<class T>
void cAsyncLoader<T>::WaitForLoadComplete()
{
    if (puiLoadingCount == 0) return;
    std::unique_lock<std::mutex> tDoneLock(ptLoadDoneMutex);
    ptLoadDoneVariable.wait(tDoneLock);
}

template<class T>
cAsyncLoader<T>::~cAsyncLoader()
{
    // Set the loader running variable to false and notify all loader threads
    pbLoaderRunning = false;
    ptLoadQueueVariable.notify_all();
}
