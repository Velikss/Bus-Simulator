#pragma once

#include <pch.hpp>
#include <scripting/duktape.h>
#include <entities/cEntityInterface.hpp>

namespace JavaScriptEntityFunctions
{
    duk_ret_t ReturnEntityCoordinates(duk_context *poContext)
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

    duk_ret_t ReturnEntityVelocity(duk_context *poContext)
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

    duk_ret_t SetEntityVelocity(duk_context *poContext)
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

    duk_ret_t ReturnEntityHeading(duk_context *poContext)
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

    duk_ret_t SetEntityHeading(duk_context *poContext)
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

    duk_ret_t ReturnEntityList(duk_context *poContext)
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
};

