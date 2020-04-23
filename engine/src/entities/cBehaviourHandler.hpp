#pragma once

#include <pch.hpp>
#include <scripting/cScriptingEngine.hpp>
#include <entities/cEntityInterface.hpp>

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

    static duk_ret_t ReturnEntityCoordinates(duk_context *poContext);

    static duk_ret_t ReturnEntityVelocity(duk_context *poContext);

    static duk_ret_t SetEntityVelocity(duk_context *poContext);

    static duk_ret_t ReturnEntityHeading(duk_context *poContext);

    static duk_ret_t SetEntityHeading(duk_context *poContext);

    static duk_ret_t ReturnEntityList(duk_context *poContext);
};

std::map<std::string, cScriptingEngine *> cBehaviourHandler::poBehaviours;

void cBehaviourHandler::AddBehaviour(std::string sBehaviourName, std::string sFileName)
{
    // Create the script engine
    cScriptingEngine *poBehaviourEngine = new cScriptingEngine();

    // Assign behaviour related functions to the engine's duktape context
    poBehaviourEngine->RegisterFunction(cBehaviourHandler::ReturnEntityCoordinates, 1, "GetEntityCoordinates");
    poBehaviourEngine->RegisterFunction(cBehaviourHandler::ReturnEntityList, 1, "GetEntityList");
    poBehaviourEngine->RegisterFunction(cBehaviourHandler::ReturnEntityVelocity, 1, "GetEntityVelocity");
    poBehaviourEngine->RegisterFunction(cBehaviourHandler::SetEntityVelocity, 3, "SetEntityVelocity");
    poBehaviourEngine->RegisterFunction(cBehaviourHandler::ReturnEntityHeading, 1, "GetEntityHeading");
    poBehaviourEngine->RegisterFunction(cBehaviourHandler::SetEntityHeading, 1, "SetEntityHeading");

    if (poBehaviourEngine->CompileJavaScriptFile(sFileName.c_str()))
        cBehaviourHandler::poBehaviours.insert({sBehaviourName, poBehaviourEngine});
}

void cBehaviourHandler::Update(BaseObject *oEntity, cEntityGroupInterface *oEntityGroup = nullptr)
{
    poBehaviours.at(psBehaviourName)->RunJavaScriptFunction("calculate", oEntity, oEntityGroup);
}

duk_ret_t cBehaviourHandler::ReturnEntityCoordinates(duk_context *poContext)
{
    if (duk_get_top(poContext) == 0)
    {
        /* throw TypeError if no arguments given */
        return DUK_RET_TYPE_ERROR;
    }

    // Get pointer from stack
    void *p = duk_get_pointer(poContext, 0);

    // Cast pointer to Entity pointer, we know it's pointing to an entity
    BaseObject *poEntity = static_cast<BaseObject *>(p);

    // Push coordinates to stack, first we'll push an empty array
    duk_idx_t ArrayIndex;
    ArrayIndex = duk_push_array(poContext);

    // Then we will fill the array with the X Y Z coordinates
    duk_push_int(poContext, poEntity->pPosition.x);
    duk_put_prop_index(poContext, ArrayIndex, 0);
    duk_push_int(poContext, poEntity->pPosition.y);
    duk_put_prop_index(poContext, ArrayIndex, 1);
    duk_push_int(poContext, poEntity->pPosition.z);
    duk_put_prop_index(poContext, ArrayIndex, 2);

    return 1;
}

duk_ret_t cBehaviourHandler::ReturnEntityVelocity(duk_context *poContext)
{
    if (duk_get_top(poContext) == 0)
    {
        /* throw TypeError if no arguments given */
        return DUK_RET_TYPE_ERROR;
    }

    // Get pointer from stack
    void *p = duk_get_pointer(poContext, 0);

    // Cast pointer to Entity pointer, we know it's pointing to an entity
    cEntityInterface *poEntity = static_cast<cEntityInterface *>(p);

    // Push coordinates to stack, first we'll push an empty array
    duk_idx_t ArrayIndex;
    ArrayIndex = duk_push_array(poContext);

    // Get Velocity and push the values to the array
    glm::vec2 velocity;
    poEntity->ReturnVelocity(&velocity);

    duk_push_int(poContext, velocity[0]);
    duk_put_prop_index(poContext, ArrayIndex, 0);
    duk_push_int(poContext, velocity[1]);
    duk_put_prop_index(poContext, ArrayIndex, 1);

    return 1;
}

duk_ret_t cBehaviourHandler::SetEntityVelocity(duk_context *poContext)
{
    if (duk_get_top(poContext) == 0)
    {
        /* throw TypeError if no arguments given */
        return DUK_RET_TYPE_ERROR;
    }

    // Get pointer from stack
    void *p = duk_to_pointer(poContext, -3);

    // Cast pointer to Entity pointer, we know it's pointing to an entity
    cEntityInterface *poEntity = static_cast<cEntityInterface *>(p);

    // Get velocity from stack
    glm::vec2 velocity(duk_to_number(poContext, -2), duk_to_number(poContext, -1));

    // Set velocity to entity
    poEntity->SetVelocity(&velocity);

    return 0;
}

duk_ret_t cBehaviourHandler::ReturnEntityHeading(duk_context *poContext)
{
    if (duk_get_top(poContext) == 0)
    {
        /* throw TypeError if no arguments given */
        return DUK_RET_TYPE_ERROR;
    }

    // Get pointer from stack
    void *p = duk_get_pointer(poContext, 0);

    // Cast pointer to Entity pointer, we know it's pointing to an entity
    cEntityInterface *poEntity = static_cast<cEntityInterface *>(p);

    // Push heading values to stack, first we'll push an empty array
    duk_idx_t ArrayIndex;
    ArrayIndex = duk_push_array(poContext);

    // Get Velocity and push the values to the array
    glm::vec2 heading;
    poEntity->ReturnHeading(&heading);

    duk_push_int(poContext, heading[0]);
    duk_put_prop_index(poContext, ArrayIndex, 0);
    duk_push_int(poContext, heading[1]);
    duk_put_prop_index(poContext, ArrayIndex, 1);

    return 1;
}

duk_ret_t cBehaviourHandler::SetEntityHeading(duk_context *poContext)
{
    if (duk_get_top(poContext) == 0)
    {
        /* throw TypeError if no arguments given */
        return DUK_RET_TYPE_ERROR;
    }

    // Get pointer from stack
    void *p = duk_to_pointer(poContext, -3);

    // Cast pointer to Entity pointer, we know it's pointing to an entity
    cEntityInterface *poEntity = static_cast<cEntityInterface *>(p);

    // Get velocity from stack
    glm::vec2 heading(duk_to_number(poContext, -2), duk_to_number(poContext, -1));

    // Set velocity to entity
    poEntity->SetHeading(&heading);

    return 0;
}

duk_ret_t cBehaviourHandler::ReturnEntityList(duk_context *poContext)
{
    if (duk_get_top(poContext) == 0)
    {
        /* throw TypeError if no arguments given */
        return DUK_RET_TYPE_ERROR;
    }

    // Get pointer from stack
    void *p = duk_get_pointer(poContext, 0);

    // Cast pointer to Entity pointer, we know it's pointing to an entity
    cEntityGroupInterface *poEntityGroup = static_cast<cEntityGroupInterface *>(p);

    // Push coordinates to stack, first we'll push an empty array
    duk_idx_t ArrayIndex;
    ArrayIndex = duk_push_array(poContext);

    // Then we will fill the array with pointers to the group members
    std::vector<cEntityInterface *> *entities;
    poEntityGroup->ReturnEntities(&entities);

    for(int i = 0; i < entities->size(); i++)
    {
        duk_push_pointer(poContext, (*entities)[i]);
        duk_put_prop_index(poContext, ArrayIndex, i);
    }

    return 1;
}


