#pragma once

#include <pch.hpp>
#include <duktape.h>
#include <vulkan/entities/IEntity.hpp>

/*
 * This namespace contains all duktape functions related to game entities, these functions are registered as javascript functions in cBehaviourHandler
 */
namespace JavaScriptEntityFunctions
{
    duk_ret_t GetEntityCoordinates(duk_context *poContext)
    {
        if (duk_get_top(poContext) == 0)
        {
            /* throw TypeError if no arguments given */
            return DUK_RET_TYPE_ERROR;
        }

        // Get pointer from stack.
        void *p = duk_get_pointer(poContext, 0);

        // Cast pointer to Entity pointer, we know it's pointing to an entity.
        cBaseObject *poEntity = static_cast<cBaseObject *>(p);

        // Push coordinates to stack, first we'll push an empty array.
        duk_idx_t ArrayIndex;
        ArrayIndex = duk_push_array(poContext);

        // Then we will fill the array with the X Y coordinates of the entity. (we take Z as Y position because vec3 is used in BaseObject)
        duk_push_number(poContext, (double)poEntity->GetPosition().x);
        duk_put_prop_index(poContext, ArrayIndex, 0);
        duk_push_number(poContext, (double)poEntity->GetPosition().z);
        duk_put_prop_index(poContext, ArrayIndex, 1);

        return 1;
    }

    duk_ret_t GetEntityTarget(duk_context *poContext)
    {
        if (duk_get_top(poContext) == 0)
        {
            /* throw TypeError if no arguments given */
            return DUK_RET_TYPE_ERROR;
        }

        // Get pointer from stack.
        void *p = duk_get_pointer(poContext, 0);

        // Cast pointer to Entity pointer, we know it's pointing to an entity.
        IEntity *poEntity = static_cast<IEntity *>(p);

        // Push coordinates to stack, first we'll push an empty array.
        duk_idx_t ArrayIndex;
        ArrayIndex = duk_push_array(poContext);

        // Then we will fill the array with the X Y coordinates of the entity's target. (we take Z as Y position because vec3 is used in BaseObject)
        duk_push_number(poContext, (double)poEntity->GetTarget().x);
        duk_put_prop_index(poContext, ArrayIndex, 0);
        duk_push_number(poContext, (double)poEntity->GetTarget().z);
        duk_put_prop_index(poContext, ArrayIndex, 1);

        return 1;
    }

    duk_ret_t GetEntityMaxSpeed(duk_context *poContext)
    {
        if (duk_get_top(poContext) == 0)
        {
            /* throw TypeError if no arguments given */
            return DUK_RET_TYPE_ERROR;
        }

        // Get pointer from stack
        void *p = duk_get_pointer(poContext, 0);

        // Cast pointer to Entity pointer, we know it's pointing to an entity
        IEntity *poEntity = static_cast<IEntity *>(p);

        // Get speed and push to the duk stack
        float speed = poEntity->GetMaxSpeed();

        duk_push_number(poContext, speed);

        return 1;
    }

    duk_ret_t GetEntityVelocity(duk_context *poContext)
    {
        if (duk_get_top(poContext) == 0)
        {
            /* throw TypeError if no arguments given */
            return DUK_RET_TYPE_ERROR;
        }

        // Get pointer from stack
        void *p = duk_get_pointer(poContext, 0);

        // Cast pointer to Entity pointer, we know it's pointing to an entity
        IEntity *poEntity = static_cast<IEntity *>(p);

        // Push coordinates to stack, first we'll push an empty array
        duk_idx_t ArrayIndex;
        ArrayIndex = duk_push_array(poContext);

        // Get Velocity and push the values to the array
        glm::vec2 velocity = poEntity->GetVelocity();

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
        IEntity *poEntity = static_cast<IEntity *>(p);

        // Get velocity from stack
        glm::vec2 velocity(duk_to_number(poContext, -2), duk_to_number(poContext, -1));

        // Set velocity to entity
        poEntity->SetVelocity(velocity);

        return 0;
    }

    duk_ret_t GetEntityList(duk_context *poContext)
    {
        if (duk_get_top(poContext) == 0)
        {
            /* throw TypeError if no arguments given */
            return DUK_RET_TYPE_ERROR;
        }

        // Get pointer from stack
        void *p = duk_get_pointer(poContext, 0);

        // Cast pointer to Entity pointer, we know it's pointing to an entity
        IEntityGroup *poEntityGroup = static_cast<IEntityGroup *>(p);

        // Push coordinates to stack, first we'll push an empty array
        duk_idx_t ArrayIndex;
        ArrayIndex = duk_push_array(poContext);

        // Fill the array with pointers to the group members
        std::vector<IEntity *> *entities;
        poEntityGroup->GetEntityList(&entities);

        for (int i = 0; i < poEntityGroup->GetEntities()->size(); i++)
        {
            duk_push_pointer(poContext, (*entities)[i]);
            duk_put_prop_index(poContext, ArrayIndex, i);
        }

        return 1;
    }

    duk_ret_t AppendEntitySteeringForce(duk_context *poContext)
    {
        if (duk_get_top(poContext) == 0)
        {
            /* throw TypeError if no arguments given */
            return DUK_RET_TYPE_ERROR;
        }

        // Get pointer from stack
        void *p = duk_to_pointer(poContext, -3);

        // Cast pointer to Entity pointer, we know it's pointing to an entity
        IEntity *poEntity = static_cast<IEntity *>(p);

        // Get SteeringForce from stack
        float fXvalue = (float) duk_to_number(poContext, -2);
        float fYvalue = (float) duk_to_number(poContext, -1);

        glm::vec2 SteeringForce(fXvalue, fYvalue);

        // Set velocity to entity
        if (!isnan(SteeringForce.x) && !isnan(SteeringForce.y))
            poEntity->AppendSteeringForce(SteeringForce);
        else
            ENGINE_WARN("Behaviour script returned NaN! " << duk_to_number(poContext, -2) << " & "
                                                          << duk_to_number(poContext, -1));

        return 0;
    }
};

