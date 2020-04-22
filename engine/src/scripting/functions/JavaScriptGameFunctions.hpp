#pragma once

#include <pch.hpp>
#include <scripting/duktape.h>

namespace JavaScriptBaseFunctions
{
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

        return 1;
    }
};

