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
    std::vector<std::string> psPathsToWatch;
    std::chrono::duration<int, std::milli> ptDuration;
    std::unordered_map<std::string, std::filesystem::file_time_type> patPaths;
    bool pbRunning = true;
    bool pbClosable = false;
public:
    enum class FileStatus
    {
        created, modified, erased
    };

    cDirectoryWatcher(std::chrono::duration<int, std::milli> ptDuration) : ptDuration{ptDuration}
    {

    }

    void AddDirectory(const std::string& sDirectoryPath);

    void AddFile(const std::string& sFilePath);

    void Start(const std::function<void(std::string, FileStatus)>& pReferencedFunction);

    void Stop();

    void RemoveFile(const string& sFilePath);

    void OnFileChanged(string sFileName, FileStatus eFileStatus);
};

void cDirectoryWatcher::Start(const std::function<void(std::string, FileStatus)>& pReferencedFunction)
{
    while (pbRunning)
    {
        std::this_thread::sleep_for(ptDuration);
        if (patPaths.size() == 0) continue;

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
        for (auto& sDirectory : psPathsToWatch)
            for (auto& tFile : std::filesystem::recursive_directory_iterator(sDirectory))
            {
                const string sFilePath = tFile.path().string();
                auto tLastWriteTime = std::filesystem::last_write_time(tFile);

                // If the file can't be found then it has been created, otherwise it has been modified.
                if (patPaths.find(sFilePath) == patPaths.end())
                {
                    patPaths[sFilePath] = tLastWriteTime;
                    pReferencedFunction(sFilePath, FileStatus::created);
                }
                else
                {
                    if (patPaths[sFilePath] != tLastWriteTime)
                    {
                        patPaths[sFilePath] = tLastWriteTime;
                        pReferencedFunction(sFilePath, FileStatus::modified);
                    }
                }
            }
    }
    pbClosable = true;
}

void cDirectoryWatcher::Stop()
{
    pbRunning = false;
    while (!pbClosable)
    { fSleep(50); }
}

void cDirectoryWatcher::AddFile(const string& sFilePath)
{
    patPaths.insert({sFilePath, std::filesystem::last_write_time(std::filesystem::path(sFilePath))});
}

void cDirectoryWatcher::RemoveFile(const string& sFilePath)
{
    patPaths.erase(sFilePath);
}

void cDirectoryWatcher::AddDirectory(const string& sDirectoryPath)
{
    for (auto& file : std::filesystem::recursive_directory_iterator(sDirectoryPath))
        patPaths[file.path().string()] = std::filesystem::last_write_time(file);
    psPathsToWatch.push_back(sDirectoryPath);
}
