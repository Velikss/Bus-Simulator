#pragma once

#include <pch.hpp>
#include <objects/BaseObject.hpp>
#include <entities/cBehaviourHandler.hpp>

class cEntity : public BaseObject
{
public:
    std::vector<cBehaviourHandler *> paBehaviourHandlers;

    cEntity(Mesh *mesh) : BaseObject(mesh)
    {

    }

    void AddBehaviour(cBehaviourHandler* &poBehaviour)
    {
        paBehaviourHandlers.push_back(poBehaviour);
    }

    virtual void Update()
    {
        for(auto& cBehaviourHandler : paBehaviourHandlers)
        {
            cBehaviourHandler->Update(this);
        }
    }

    static duk_ret_t GetEntityX(duk_context *poContext)
    {
        if (duk_get_top(poContext) == 0)
        {
            /* throw TypeError if no arguments given */
            return DUK_RET_TYPE_ERROR;
        }

        // Get pointer from stack
        void *p = duk_get_pointer(poContext, 0);

        // Cast pointer to Entity pointer
        cEntity *poEntity = static_cast<cEntity *>(p);

        // Return value
        duk_push_int(poContext, poEntity->pos[0]);

        std::cout << "Return value: " << poEntity->pos[0];

        return 1;
    }
};

