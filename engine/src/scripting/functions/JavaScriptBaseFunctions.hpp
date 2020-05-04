#pragma once

#include <pch.hpp>
#include <duktape.h>

/*
 * This namespace contains basic javascript functions which can be used when making a script for the game engine, these functions are registered as javascript functions in the scripting engine.
 */
namespace JavaScriptBaseFunctions
{
    // Prints input to console.
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

    // Prints text on a new console line.
    static duk_ret_t println(duk_context *poContext)
    {
        if (duk_get_top(poContext) == 0)
        {
            /* throw TypeError if no arguments given */
            return DUK_RET_TYPE_ERROR;
        }

        std::cout << duk_to_string(poContext, 0) << std::endl;

        return 0;
    }
}
