#pragma once
#include <pch.hpp>
#include <scripting/cScriptingEngine.hpp>
#include <objects/BaseObject.hpp>

class cBehaviourHandler
{
protected:
    static std::map<std::string, cScriptingEngine*> poBehaviours;
    std::string psBehaviourName;
public:
     cBehaviourHandler(std::string sBehaviourName)
    {
        if(poBehaviours.find(sBehaviourName) == poBehaviours.end())
        {
            assert(false); // Couldn't find specified behaviour.
        }

        psBehaviourName = sBehaviourName;
    }

    virtual void Update(BaseObject* oEntity);

    static void AddBehaviour(std::string sBehaviourName, std::string sFileName);

    static void UpdateEngine(std::string sBehaviourName, std::string sFileName);
};

std::map<std::string, cScriptingEngine*> cBehaviourHandler::poBehaviours;

void cBehaviourHandler::AddBehaviour(std::string sBehaviourName, std::string sFileName)
{
    cScriptingEngine *poBehaviourScript = new cScriptingEngine();

    if(poBehaviourScript->CompileJavaScriptFile(sFileName.c_str()))
        cBehaviourHandler::poBehaviours.insert({ sBehaviourName, poBehaviourScript});
}

void cBehaviourHandler::Update(BaseObject* oEntity)
{
    poBehaviours.at(psBehaviourName)->RunJavaScriptFunction("execute", oEntity, nullptr);
}


