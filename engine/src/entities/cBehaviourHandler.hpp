#pragma once

#include <pch.hpp>
#include <scripting/cScriptingEngine.hpp>
//#include <entities/cEntityInterface.hpp>
#include <scripting/functions/JavaScriptEntityFunctions.hpp>

class cBehaviourHandler
{
protected:
    static std::map<std::string, cScriptingEngine *> poBehaviours;
    std::string psBehaviourName;
public:
    cBehaviourHandler(std::string sBehaviourName)
    {
        if (poBehaviours.find(sBehaviourName) == poBehaviours.end())
        {
            assert(false); // Couldn't find specified behaviour.
        }

        psBehaviourName = sBehaviourName;
    }

    static void UpdateEngine(std::string sBehaviourName, std::string sFileName);

    static void AddBehaviour(std::string sBehaviourName, std::string sFileName);

    virtual void Update(BaseObject *oEntity, cEntityGroupInterface *oEntityGroup);
};

std::map<std::string, cScriptingEngine *> cBehaviourHandler::poBehaviours;

void cBehaviourHandler::AddBehaviour(std::string sBehaviourName, std::string sFileName)
{
    // Create the script engine
    cScriptingEngine *poBehaviourEngine = new cScriptingEngine();

    // Assign behaviour related functions to the engine's duktape context
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::ReturnEntityCoordinates, 1, "GetEntityCoordinates");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::ReturnEntityList, 1, "GetEntityList");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::ReturnEntityVelocity, 1, "GetEntityVelocity");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::SetEntityVelocity, 3, "SetEntityVelocity");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::ReturnEntityHeading, 1, "GetEntityHeading");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::SetEntityHeading, 1, "SetEntityHeading");

    if (poBehaviourEngine->CompileJavaScriptFile(sFileName.c_str()))
        cBehaviourHandler::poBehaviours.insert({sBehaviourName, poBehaviourEngine});
}

void cBehaviourHandler::Update(BaseObject *oEntity, cEntityGroupInterface *oEntityGroup = nullptr)
{
    poBehaviours.at(psBehaviourName)->RunJavaScriptFunction("calculate", oEntity, oEntityGroup);
}
