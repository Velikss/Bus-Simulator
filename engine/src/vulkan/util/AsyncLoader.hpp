#pragma once

#include <pch.hpp>
#include <mutex>
#include <condition_variable>
#include <atomic>

template<class T>
class cAsyncLoader
{
private:
    // Vector with all the active loader threads
    std::vector<std::thread*> papThreads;
    // Indicates if the loader threads should be running
    // When false, all running loader threads will terminate
    // after completing their work and any waiting loader
    // threads will terminate after being notified
    bool pbLoaderRunning = true;

    // Queue holding all the objects waiting to be loaded
    std::queue<T*> papLoadQueue;
    // Mutex and condition variable protecting the load queue
    std::mutex ptLoadQueueMutex;
    std::condition_variable ptLoadQueueVariable;

    // Mutex and condition variable that will be notified when
    // the loader threads finish loading everything in the queue
    std::mutex ptLoadDoneMutex;
    std::condition_variable ptLoadDoneVariable;

    // Amount of objects waiting to be loaded
    std::atomic_uint puiLoadingCount = 0;

public:
    // Create a new AsyncLoader and start a number of loading threads
    cAsyncLoader(uint uiThreadCount);
    virtual ~cAsyncLoader();

    // Start loading an object (non-blocking)
    void LoadAsync(T* pObject);
    // Wait until all objects are finished loading (blocking)
    void WaitForLoadComplete();

protected:
    // Callback that will be called from a loader thread
    // for every object to load
    // The order in which the load callback will be called
    // is undefined and may not match the order in which
    // the Load() method has been called
    virtual void LoadCallback(T* pObject) = 0;

private:
    // Start a loader thread. Must be called on a new
    // thread as it will instantly block the current thread
    void StartLoaderThread();
};

template<class T>
cAsyncLoader<T>::cAsyncLoader(uint uiThreadCount)
{
    // Create the specified number of loading threads
    for (uint uiIndex = 0; uiIndex < uiThreadCount; uiIndex++)
    {
        papThreads.push_back(new std::thread(&cAsyncLoader::StartLoaderThread, this));
    }
}

template<class T>
void cAsyncLoader<T>::StartLoaderThread()
{
    // Keep looping while the loader threads should run
    while (pbLoaderRunning)
    {
        // Aqcuire the load queue mutex
        std::unique_lock<std::mutex> tLoadLock(ptLoadQueueMutex);
        // If the load queue is empty, wait until notified (blocks current thread)
        if (papLoadQueue.empty()) ptLoadQueueVariable.wait(tLoadLock);
        // If the loader threads should stop running, return
        if (!pbLoaderRunning) return;

        // Grab the object from the front of the load queue
        T* pObject = papLoadQueue.front();
        papLoadQueue.pop();

        // Release the load queue mutex
        tLoadLock.unlock();

        // Perform the load operation(s)
        LoadCallback(pObject);

        // Decrement the amount of objects waiting to load
        puiLoadingCount--;
        // If there are no objects left to load, notify
        // all threads that are waiting on the done variable
        // (see the WaitForLoadComplete() method)
        if (puiLoadingCount == 0) ptLoadDoneVariable.notify_all();
    }
}

template<class T>
void cAsyncLoader<T>::LoadAsync(T* pObject)
{
    // Aqcuire the load queue mutex
    ptLoadQueueMutex.lock();
    // Add the object to the load queue
    papLoadQueue.push(pObject);
    // Increment the number of objects waiting to load
    puiLoadingCount++;
    // Release the load queue mutex
    ptLoadQueueMutex.unlock();
    // Notify one of the waiting load threads (if any are waiting)
    ptLoadQueueVariable.notify_one();
}

template<class T>
void cAsyncLoader<T>::WaitForLoadComplete()
{
    // If there are no more objects waiting to load, we can return immediately
    if (puiLoadingCount == 0) return;
    // Wait until the done variable is notified (blocks current thread)
    std::unique_lock<std::mutex> tDoneLock(ptLoadDoneMutex);
    ptLoadDoneVariable.wait(tDoneLock);
}

template<class T>
cAsyncLoader<T>::~cAsyncLoader()
{
    // Set the loader running variable to false and notify all loader threads
    pbLoaderRunning = false;
    ptLoadQueueVariable.notify_all();

    // Join and delete all loader threads
    for (std::thread* pThread : papThreads)
    {
        pThread->join();
        delete pThread;
    }
}
