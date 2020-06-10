#pragma once

#include <pch.hpp>
#include <duktape.h>
#include <fstream>
#include <scripting/functions/JavaScriptBaseFunctions.hpp>

class cScriptingEngine
{
public:
    duk_context* ppoContext;

    cScriptingEngine()
    {
        ppoContext = duk_create_heap_default();
        if (!ppoContext)
        {
            printf("Failed to create a Duktape heap.\n");
            exit(1);
        }

        Init();
    }

    ~cScriptingEngine()
    {
        duk_destroy_heap(ppoContext);
    }

    void Init();

    void RegisterFunction(duk_c_function fnfunc, duk_idx_t iArgs, const char* psKey);

    bool CompileJavaScriptFile(const char* psFilename);

    bool RunJavaScriptFunction(const char* psFunctionName, void* pArga, void* pArgb);

};

void cScriptingEngine::Init()
{
    // Link all the C functions to use in javascript
    duk_push_c_function(ppoContext, JavaScriptBaseFunctions::print, 1);
    duk_put_global_string(ppoContext, "print");

    duk_push_c_function(ppoContext, JavaScriptBaseFunctions::println, 1);
    duk_put_global_string(ppoContext, "println");
}

void cScriptingEngine::RegisterFunction(duk_c_function fnFunc, duk_idx_t iArgs, const char* psKey)
{
    duk_push_c_function(ppoContext, fnFunc, iArgs);
    duk_put_global_string(ppoContext, psKey);
}

bool cScriptingEngine::CompileJavaScriptFile(const char* psFilename)
{
    bool bSucces = false;

    // Read file as string
    std::ifstream oFileStream(psFilename);
    if (!oFileStream.is_open())
    {
        ENGINE_WARN(psFilename << " could not be found.");
        return bSucces;
    }
    std::string strFileContent((std::istreambuf_iterator<char>(oFileStream)),
                               std::istreambuf_iterator<char>());

    // Compile JavaScript to bytecode
    if (duk_pcompile_string(ppoContext, 0, strFileContent.c_str()) != 0)
    {
        // Error during compiling
        ENGINE_WARN("Scripting compile failed (" << psFilename << ")");
        ENGINE_WARN(duk_safe_to_string(ppoContext, -1));
    }
    else
    {
        // No error so code can be evaluated
        if (duk_pcall(ppoContext, 0) == DUK_EXEC_SUCCESS)
        {
            bSucces = true;
        }
    }
    duk_pop(ppoContext);

    return bSucces;
}

bool cScriptingEngine::RunJavaScriptFunction(const char* psFunctionName, void* pArga = nullptr, void* pArgb = nullptr)
{
    bool bReturnVal = true;

    // Get a reference to the named JS function
    if (duk_get_global_string(ppoContext, psFunctionName))
    {
        // Function found, push the args

        duk_push_pointer(ppoContext, pArga);
        duk_push_pointer(ppoContext, pArgb);

        // Use pcall - this lets you catch and handle any errors
        if (duk_pcall(ppoContext, 2) != DUK_EXEC_SUCCESS)
        {
            // An error occurred - display a stack trace
            duk_get_prop_string(ppoContext, -1, "stack");
            ENGINE_WARN(duk_safe_to_string(ppoContext, -1));
            bReturnVal = false;
        }
        else
        {
            // Function executed successfully
            bReturnVal = true;
        }
    }
    else
    {
        ENGINE_WARN("Scripting function called but not found! (" << psFunctionName << ")");
        bReturnVal = false;
    }

    duk_pop(ppoContext); // pop result

    return bReturnVal;
}



