#pragma once

#include <pch.hpp>
#include <scripting/cScriptingEngine.hpp>

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

    virtual void Update(BaseObject *oEntity);

    static void AddBehaviour(std::string sBehaviourName, std::string sFileName);

    static void UpdateEngine(std::string sBehaviourName, std::string sFileName);

    static duk_ret_t GetEntityX(duk_context *poContext);
};

std::map<std::string, cScriptingEngine *> cBehaviourHandler::poBehaviours;

void cBehaviourHandler::AddBehaviour(std::string sBehaviourName, std::string sFileName)
{
    // Create the script engine
    cScriptingEngine *poBehaviourEngine = new cScriptingEngine();

    // Assign behaviour related functions to the engine's duktape context
    poBehaviourEngine->RegisterFunction(cBehaviourHandler::GetEntityX, 1, "GetEntityX");

    if (poBehaviourEngine->CompileJavaScriptFile(sFileName.c_str()))
        cBehaviourHandler::poBehaviours.insert({sBehaviourName, poBehaviourEngine});
}

void cBehaviourHandler::Update(BaseObject *oEntity)
{
    poBehaviours.at(psBehaviourName)->RunJavaScriptFunction("execute", oEntity, nullptr);
}

duk_ret_t cBehaviourHandler::GetEntityX(duk_context *poContext)
{
    if (duk_get_top(poContext) == 0)
    {
        /* throw TypeError if no arguments given */
        return DUK_RET_TYPE_ERROR;
    }

    // Get pointer from stack
    void *p = duk_get_pointer(poContext, 0);

    // Cast pointer to Entity pointer
    BaseObject *poEntity = static_cast<BaseObject *>(p);

    // Return value
    duk_push_int(poContext, poEntity->pos[0]);

    std::cout << "Return value: " << poEntity->pos[0];

    return 1;
}


