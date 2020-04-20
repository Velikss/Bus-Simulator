#pragma once

#include <pch.hpp>
#include <scripting/duktape.h>
#include <scripting/functions/JavaScriptBaseFunctions.hpp>

class cScriptingEngine
{
public:
    duk_context* ctx;

    cScriptingEngine() {
        ctx = duk_create_heap_default();
        if (!ctx) {
            printf("Failed to create a Duktape heap.\n");
            exit(1);
        }

        Init();
    }

    ~cScriptingEngine() {
        duk_destroy_heap(ctx);
    }

    void Init();

    int Interpret(string & code);

    void PushFileAsString(duk_context *ctx, const char *filename);

    void RunFile(const char *filename);

};

void cScriptingEngine::Init() {
    duk_push_c_function(ctx, JavaScriptBaseFunctions::print,1);
    duk_put_global_string(ctx, "print");

    duk_push_c_function(ctx, JavaScriptBaseFunctions::println, 1);
    duk_put_global_string(ctx, "println");
}

int cScriptingEngine::Interpret(string &code) {
        duk_eval_string_noresult(ctx, code.c_str());
        return 0;
}

void cScriptingEngine::PushFileAsString(duk_context *ctx, const char *filename) {
        FILE *f;
        size_t len;
        char buf[16384];

        f = fopen(filename, "rb");
        if (f) {
            len = fread((void *) buf, 1, sizeof(buf), f);
            fclose(f);
            duk_push_lstring(ctx, (const char *) buf, (duk_size_t) len);
        } else {
            duk_push_undefined(ctx);
        }
    }

    void cScriptingEngine::RunFile(const char *filename) {
        PushFileAsString(ctx, filename);
        if (duk_peval(ctx) != 0) {
            printf("Error: %s\n", duk_safe_to_string(ctx, -1));
            return;
        }
        //duk_pop(ctx);  /* ignore result */
}

