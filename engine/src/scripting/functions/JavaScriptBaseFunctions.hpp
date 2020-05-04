#pragma once

#include <pch.hpp>
#include <scripting/duktape.h>

namespace JavaScriptBaseFunctions
{
    static duk_ret_t print(duk_context *poContext)
    {
        if (duk_get_top(poContext) == 0)
        {
            /* throw TypeError if no arguments given */
            return DUK_RET_TYPE_ERROR;
        }

        std::cout << duk_to_string(poContext, -1);

        return 0;
    }

    static duk_ret_t println(duk_context *poContext)
    {
        if (duk_get_top(poContext) == 0)
        {
            /* throw TypeError if no arguments given */
            return DUK_RET_TYPE_ERROR;
        }

        std::cout << std::endl << duk_to_string(poContext, 0);

        return 0;
    }
}