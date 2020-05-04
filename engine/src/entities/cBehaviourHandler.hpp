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

    virtual glm::vec2 Update(BaseObject *oEntity, cEntityGroupInterface *oEntityGroup);
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
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::ReturnEntityCoordinates, 1, "GetEntityCoordinates");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::ReturnEntityMass, 1, "GetEntityMass");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::ReturnEntityMaxSpeed, 1, "GetEntityMaxSpeed");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::ReturnEntityList, 1, "GetEntityList");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::ReturnEntityVelocity, 1, "GetEntityVelocity");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::SetEntityVelocity, 3, "SetEntityVelocity");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::ReturnEntityHeading, 1, "GetEntityHeading");
    poBehaviourEngine->RegisterFunction(JavaScriptEntityFunctions::SetEntityHeading, 3, "SetEntityHeading");

    /* Compile the behaviours' script (it will be placed as bytecode on the engines' stack)
     * If it compiles successful, add the behaviour with it's engine to poBehaviour.
     */
    if (poBehaviourEngine->CompileJavaScriptFile(sFileName.c_str()))
        cBehaviourHandler::poBehaviours.insert({sBehaviourName, poBehaviourEngine});
}

/*
 * Calls calculate function from the engines' stack, then returns steering force.
 */
glm::vec2 cBehaviourHandler::Update(BaseObject *oEntity, cEntityGroupInterface *oEntityGroup = nullptr)
{
    poBehaviours.at(psBehaviourName)->RunJavaScriptFunction("calculate", oEntity, oEntityGroup);
    //TODO get steeringforce from Js and return it
    return glm::vec2(0, 0);
}
