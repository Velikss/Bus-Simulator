#pragma once

#include <pch.hpp>
#include <filesystem>
#include <scripting/cScriptingEngine.hpp>
#include <scripting/functions/JavaScriptEntityFunctions.hpp>
#include "../../../../server/src/DirectoryWatcher.hpp"

class cBehaviourHandler
{
protected:
    static std::map<std::string, std::shared_ptr<cScriptingEngine>> poBehaviours;
    static cDirectoryWatcher *ppoDirectoryWatcher;
    std::string psBehaviourName;
public:
    cBehaviourHandler(const std::string &sBehaviourName)
    {
        if (poBehaviours.find(sBehaviourName) == poBehaviours.end())
        {
            assert(false); // Couldn't find specified behaviour.
        }

        psBehaviourName = sBehaviourName;
    }

    static void Init();

    static void AddBehavioursFromDirectory(const std::string &sDirectoryPath);

    static void UpdateEngine(const std::string &sBehaviourName, const std::string &sFileName);

    static void AddBehaviour(const std::string &sBehaviourName, const std::string &sFileName);

    virtual void Update(cBaseObject *oEntity, IEntityGroup *oEntityGroup = nullptr);

    static void OnFileChanged(string sFilePath, cDirectoryWatcher::FileStatus eFileStatus);
};

cDirectoryWatcher *cBehaviourHandler::ppoDirectoryWatcher = nullptr;
std::map<std::string, std::shared_ptr<cScriptingEngine>> cBehaviourHandler::poBehaviours;

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

void cBehaviourHandler::AddBehavioursFromDirectory(const std::string &sDirectoryPath)
{
    for (auto &p: std::filesystem::directory_iterator(sDirectoryPath))
    {
        std::string sPath = p.path().string();
        std::vector<std::string> soSplit = split(sPath, ".");
        std::vector<std::string> soSplit2 = split(soSplit[0], "/");
        AddBehaviour(soSplit2[soSplit2.size() - 1], sPath);
    }

    ppoDirectoryWatcher->AddDirectory(sDirectoryPath);
}

/*
 * Add behaviour to poBehaviours.
 */
void cBehaviourHandler::AddBehaviour(const std::string &sBehaviourName, const std::string &sFileName)
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

    /* Compile the behaviours' script (it will be placed as bytecode on the engines' stack)
     * If it compiles successful, add the behaviour with it's engine to poBehaviour.
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
void cBehaviourHandler::Update(cBaseObject *oEntity, IEntityGroup *oEntityGroup)
{
    poBehaviours.at(psBehaviourName)->RunJavaScriptFunction("calculate", oEntity, oEntityGroup);
}

void cBehaviourHandler::OnFileChanged(std::string sFilePath, cDirectoryWatcher::FileStatus eFileStatus)
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

            /* Compile the behaviours' script (it will be placed as bytecode on the engines' stack)
             * If it compiles successful, add the behaviour with it's engine to poBehaviour.
             */
            if (poTempBehaviourEngine->CompileJavaScriptFile(sFilePath.c_str()))
            {
                poBehaviours.at(sFileName.back()) = poTempBehaviourEngine;
                ENGINE_LOG("Script succesfully compiled (" << sFilePath << ")");
            } else
            {
                ENGINE_WARN("Compile of " << sFilePath << " failed.");
            }
            break;
        }
        case cDirectoryWatcher::FileStatus::erased:
        {
            //TODO: support for removing behaviour files
        }
    }
}
