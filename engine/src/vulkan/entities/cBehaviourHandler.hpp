#pragma once

#include <pch.hpp>
#include <scripting/cScriptingEngine.hpp>
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

    virtual void Update(cBaseObject *oEntity, cEntityGroupInterface *oEntityGroup);
};

std::map<std::string, cScriptingEngine *> cBehaviourHandler::poBehaviours;

/*
 * Add behaviour to poBehaviours.
 */
void cBehaviourHandler::AddBehaviour(std::string sBehaviourName, std::string sFileName)
{
    // Create the script engine
    cScriptingEngine *poBehaviourEngine = new cScriptingEngine();

    // Register the needed duktape functions to the engine's duktape context
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::GetEntityCoordinates, 1, "GetEntityCoordinates");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::GetEntityTarget, 1, "GetEntityTarget");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::GetEntityMaxSpeed, 1, "GetEntityMaxSpeed");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::GetEntityList, 1, "GetEntityList");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::GetEntityVelocity, 1, "GetEntityVelocity");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::SetEntityVelocity, 3, "SetEntityVelocity");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::AppendEntitySteeringForce, 3, "SetEntitySteeringForce");

    /* Compile the behaviours' script (it will be placed as bytecode on the engines' stack)
     * If it compiles successful, add the behaviour with it's engine to poBehaviour.
     */
    if (poBehaviourEngine->CompileJavaScriptFile(sFileName.c_str()))
        cBehaviourHandler::poBehaviours.insert({sBehaviourName, poBehaviourEngine});
}

/*
 * Calls calculate function from the engines' stack, which will calculate a steering force and append it to the entity's steering force..
 */
void cBehaviourHandler::Update(cBaseObject *oEntity, cEntityGroupInterface *oEntityGroup = nullptr)
{
    poBehaviours.at(psBehaviourName)->RunJavaScriptFunction("calculate", oEntity, oEntityGroup);
}
