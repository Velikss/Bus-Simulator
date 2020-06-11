#pragma once

#include <pch.hpp>
#include <filesystem>
#include <scripting/ScriptingEngine.hpp>
#include <scripting/functions/JavaScriptEntityFunctions.hpp>
#include <vulkan/util/DirectoryWatcher.hpp>

class cBehaviourHandler
{
protected:
    static std::map<std::string, std::shared_ptr<cScriptingEngine>> poBehaviours;
    static cDirectoryWatcher* ppoDirectoryWatcher;
    std::string psBehaviourName;
public:
    cBehaviourHandler(const std::string& sBehaviourName)
    {
        if (poBehaviours.find(sBehaviourName) == poBehaviours.end())
        {
            assert(false); // Couldn't find specified behaviour.
        }

        psBehaviourName = sBehaviourName;
    }

    static void Init();

    static void AddBehavioursFromDirectory(const std::string& sDirectoryPath);

    static void AddBehaviour(const std::string& sBehaviourName, const std::string& sFileName);

    virtual void Update(cBaseObject* oEntity, IEntityGroup* oEntityGroup = nullptr);

    static void OnFileChanged(const string& sFilePath, cDirectoryWatcher::FileStatus eFileStatus);
};

cDirectoryWatcher* cBehaviourHandler::ppoDirectoryWatcher = nullptr;
std::map<std::string, std::shared_ptr<cScriptingEngine>> cBehaviourHandler::poBehaviours;

/*
 * Initialize the behaviour handler, sets up a directory watcher on it's own thread.
 */
void cBehaviourHandler::Init()
{
    if (!ppoDirectoryWatcher)
    {
        ppoDirectoryWatcher = new cDirectoryWatcher(std::chrono::milliseconds(5000));

        (std::thread([&]
                     {
                         std::function<void(std::string, cDirectoryWatcher::FileStatus)> _OnFileChanged = std::bind(
                                 &cBehaviourHandler::OnFileChanged, std::placeholders::_1, std::placeholders::_2);
                         ppoDirectoryWatcher->Start(_OnFileChanged);
                     })).detach();
    }

}

/*
 * Function to start the directory watchers on the given directory.
 */
void cBehaviourHandler::AddBehavioursFromDirectory(const std::string& sDirectoryPath)
{
    for (auto& p: std::filesystem::directory_iterator(sDirectoryPath))
    {
        std::string sPath = p.path().string();
        std::vector<std::string> soSplit = split(sPath, ".");
#if defined(LINUX)
        std::vector<std::string> soSplit2 = split(soSplit[0], "/");
#elif defined(WINDOWS)
        std::vector<std::string> soSplit2 = split(soSplit[0], "\\");
#endif
        ENGINE_WARN(soSplit2[soSplit2.size() - 1]);
        AddBehaviour(soSplit2[soSplit2.size() - 1], sPath);
    }

    ppoDirectoryWatcher->AddDirectory(sDirectoryPath);
}

/*
 * Add behaviour to poBehaviours. Creates a ScriptingEngine with it's own duktape context,
 * then compiles the given script and adds it to the directory watcher.
 */
void cBehaviourHandler::AddBehaviour(const std::string& sBehaviourName, const std::string& sFileName)
{
    // Create the script engine
    std::shared_ptr<cScriptingEngine> poBehaviourEngine = std::make_shared<cScriptingEngine>();

    // Register the needed duktape functions to the engine's duktape context
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::GetEntityCoordinates, 1, "GetEntityCoordinates");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::GetEntityTarget, 1, "GetEntityTarget");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::GetEntityMaxSpeed, 1, "GetEntityMaxSpeed");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::GetEntityList, 1, "GetEntityList");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::GetEntityVelocity, 1, "GetEntityVelocity");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::SetEntityVelocity, 3, "SetEntityVelocity");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::AppendEntitySteeringForce, 3,
                                        "SetEntitySteeringForce");

    /*
     * Compile the behaviours' script (it will be placed as bytecode on the engines' stack)
     * If it compiles successful, add the behaviour with it's engine to poBehaviour and add it to the directory watcher.
     */
    if (poBehaviourEngine->CompileJavaScriptFile(sFileName.c_str()))
    {
        cBehaviourHandler::poBehaviours.insert({sBehaviourName, poBehaviourEngine});
        ppoDirectoryWatcher->AddFile(sFileName);
        ENGINE_LOG("Script succesfully compiled (" << sFileName << ")");
    }
}

/*
 * Calls calculate function from the engines' stack, which will calculate a steering force and append it to the entity's steering force..
 */
void cBehaviourHandler::Update(cBaseObject* oEntity, IEntityGroup* oEntityGroup)
{
    poBehaviours.at(psBehaviourName)->RunJavaScriptFunction("calculate", oEntity, oEntityGroup);
}

/*
 * Runs if the directory watcher detects changes. Will add behaviours, recompile existing behaviours and remove them if that's the case.
 */
void cBehaviourHandler::OnFileChanged(const std::string& sFilePath, cDirectoryWatcher::FileStatus eFileStatus)
{
    std::vector<std::string> sFileName = split(split(sFilePath, ".")[0], "/");

    switch (eFileStatus)
    {
        case cDirectoryWatcher::FileStatus::created:
        {
            AddBehaviour(sFileName.back(), sFilePath);
            ENGINE_LOG("Added behaviour script (" << sFilePath << ")");
            break;
        }
        case cDirectoryWatcher::FileStatus::modified:
        {
            // Create the script engine
            std::shared_ptr<cScriptingEngine> poTempBehaviourEngine = std::make_shared<cScriptingEngine>();

            // Register the needed duktape functions to the engine's duktape context
            poTempBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::GetEntityCoordinates, 1,
                                                    "GetEntityCoordinates");
            poTempBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::GetEntityTarget, 1, "GetEntityTarget");
            poTempBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::GetEntityMaxSpeed, 1,
                                                    "GetEntityMaxSpeed");
            poTempBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::GetEntityList, 1, "GetEntityList");
            poTempBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::GetEntityVelocity, 1,
                                                    "GetEntityVelocity");
            poTempBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::SetEntityVelocity, 3,
                                                    "SetEntityVelocity");
            poTempBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::AppendEntitySteeringForce, 3,
                                                    "SetEntitySteeringForce");

            /*
             * If the script compiles successfully, replace the old scripting engine with the new one.
             */
            if (poTempBehaviourEngine->CompileJavaScriptFile(sFilePath.c_str()))
            {
                poBehaviours.at(sFileName.back()) = poTempBehaviourEngine;
                ENGINE_LOG("Script succesfully compiled (" << sFilePath << ")");
            }
            else
            {
                ENGINE_WARN("Compile of " << sFilePath << " failed. Changes made to the script are not applied.");
            }
            break;
        }
        default:
            break;
    }
}
