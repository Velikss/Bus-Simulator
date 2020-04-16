#pragma once
#include <pch.hpp>
#include <filesystem>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>

class cDirectoryWatcher
{
    std::string psPathToWatch;
    std::chrono::duration<int, std::milli> ptDuration;
    std::unordered_map<std::string, std::filesystem::file_time_type> patPaths;
    bool pbRunning = true;
    bool pbClosable = false;
public:
    enum class FileStatus { created, modified, erased };

    cDirectoryWatcher(std::string psPathToWatch, std::chrono::duration<int, std::milli> ptDuration) : psPathToWatch{ psPathToWatch }, ptDuration{ ptDuration } {
        for (auto& file : std::filesystem::recursive_directory_iterator(psPathToWatch))
            patPaths[file.path().string()] = std::filesystem::last_write_time(file);
    }

    void Start(const std::function<void(std::string, FileStatus)>& pReferencedFunction);

    void Stop();
};

void cDirectoryWatcher::Start(const std::function<void(std::string, FileStatus)>& pReferencedFunction)
{
    while (pbRunning)
    {
        std::this_thread::sleep_for(ptDuration);

        // Check if any files have been removed, thus remove from the watchlist too.
        auto tPath = patPaths.begin();
        while (tPath != patPaths.end())
        {
            if (!std::filesystem::exists(tPath->first))
            {
                pReferencedFunction(tPath->first, FileStatus::erased);
                tPath = patPaths.erase(tPath);
            }
            else
                tPath++;
        }

        // Check if a file was created or modified
        for (auto &tFile : std::filesystem::recursive_directory_iterator(psPathToWatch))
        {
            auto tLastWriteTime = std::filesystem::last_write_time(tFile);

            // If the file can't be found then it has been created, otherwise it has been modified.
            if (patPaths.find(tFile.path().string()) == patPaths.end())
            {
                patPaths[tFile.path().string()] = tLastWriteTime;
                pReferencedFunction(tFile.path().string(), FileStatus::created);
            }
            else
            {
                if (patPaths[tFile.path().string()] != tLastWriteTime)
                {
                    patPaths[tFile.path().string()] = tLastWriteTime;
                    pReferencedFunction(tFile.path().string(), FileStatus::modified);
                }
            }
        }
    }
    pbClosable = true;
}

void cDirectoryWatcher::Stop()
{
    pbRunning = false;
    while(!pbClosable){}
}