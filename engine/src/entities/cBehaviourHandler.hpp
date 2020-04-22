#pragma once
#include <pch.hpp>
class cScriptingEngine;
#include <scripting/cScriptingEngine.hpp>
#include <objects/BaseObject.hpp>

class cBehaviourHandler
{
protected:
    static std::map<std::string, cScriptingEngine*> poBehaviours;
    std::string psBehaviourName;
public:
    static void AddBehaviour(std::string sBehaviourName, std::string sFileName);
    static void Update(std::string sBehaviourName, std::string sFileName);

    cBehaviourHandler(std::string sBehaviourName)
    {
        if(poBehaviours.find(sBehaviourName) == poBehaviours.end())
        {
            assert(false); // Couldn't find specified behaviour.
        }

        psBehaviourName = sBehaviourName;
    }

    virtual void Update(BaseObject &oEntity)
    {
        oEntity.Update();
    }
//
//    void ExecuteBehaviour(const char *pstrBehaviourScript, cEntity *poEntity, std::vector<cEntity *> Entities);
//
//
//    void cScriptingEngine::ExecuteBehaviour(const char *pstrBehaviourScript, cEntity *poEntity, std::vector<cEntity *> Entities)
//    {
//
//        CompileJavaScriptFile(pstrBehaviourScript);
//
//        RunJavaScriptFunction(ppoContext, "execute", poEntity, &Entities);
//
//    }

};

static std::map<std::string, cScriptingEngine*> poBehaviours;
