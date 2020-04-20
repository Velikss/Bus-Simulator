#pragma once

#include <pch.hpp>
#include <scripting/duktape.h>

namespace JavaScriptBaseFunctions {
    static duk_ret_t print(duk_context *ctx) {
        if (duk_get_top(ctx) == 0) {
            /* throw TypeError if no arguments given */
            return DUK_RET_TYPE_ERROR;
        }

        std::cout << duk_to_string(ctx, 0);

        return 0;
    }

    static duk_ret_t println(duk_context *ctx) {
        if (duk_get_top(ctx) == 0) {
            /* throw TypeError if no arguments given */
            return DUK_RET_TYPE_ERROR;
        }

        std::cout << std::endl << duk_to_string(ctx, 0);

        return 0;
    }
}

