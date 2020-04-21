#pragma once

#include <pch.hpp>
#include <scripting/duktape.h>
#include <scripting/functions/JavaScriptBaseFunctions.hpp>
#include <fstream>
#include <entities/cEntity.hpp>

class cScriptingEngine
{
public:
    duk_context *ppoContext;

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

    void ExecuteBehaviour(const char *pstrBehaviourScript, cEntity *poEntity, std::vector<cEntity *> Entities);

    bool CompileJavaScriptFile(const char *filename);

    bool RunJavaScriptFunction(duk_context *poContext, const char *pstrFunctionName, void *pArga, void *pArgb);

};

void cScriptingEngine::Init()
{
    // Link all the C functions to use in javascript

    duk_push_c_function(ppoContext, JavaScriptBaseFunctions::print, 1);
    duk_put_global_string(ppoContext, "print");

    duk_push_c_function(ppoContext, JavaScriptBaseFunctions::println, 1);
    duk_put_global_string(ppoContext, "println");

    duk_push_c_function(ppoContext, JavaScriptBaseFunctions::GetEntityX, 1);
    duk_put_global_string(ppoContext, "GetEntityX");
}

void cScriptingEngine::ExecuteBehaviour(const char *pstrBehaviourScript, cEntity *poEntity, std::vector<cEntity *> Entities)
{

    CompileJavaScriptFile(pstrBehaviourScript);

    RunJavaScriptFunction(ppoContext, "execute", poEntity, &Entities);

}

bool cScriptingEngine::CompileJavaScriptFile(const char *pstrFilename)
{
    bool bSucces = false;

    // Read file as string
    std::ifstream oFileStream(pstrFilename);
    if (!oFileStream.is_open())
    {
        std::cout << pstrFilename << " could not be found." << std::endl;
        return -1;
    }
    std::string strFileContent((std::istreambuf_iterator<char>(oFileStream)),
                               std::istreambuf_iterator<char>());

    // Compile JavaScript to bytecode
    if (duk_pcompile_string(ppoContext, 0, strFileContent.c_str()) != 0)
    {
        // Error during compiling
        std::cout << "JS compile failed" << std::endl;
        std::cout << duk_safe_to_string(ppoContext, -1);
    }
    else
    {
        // No error so code can be evaluated
        duk_pcall(ppoContext, 0);

        bSucces = true;
    }
    duk_pop(ppoContext);

    return bSucces;
}

bool cScriptingEngine::RunJavaScriptFunction(duk_context *poContext, const char *pstrFunctionName, void *pArga, void *pArgb)
{
    bool bReturnVal;

    // Get a reference to the named JS function
    if (duk_get_global_string(poContext, pstrFunctionName))
    {
        // Function found, push the args

        duk_push_pointer(poContext, pArga);
        duk_push_pointer(poContext, pArgb);

        // Use pcall - this lets you catch and handle any errors
        if (duk_pcall(poContext, 2) != DUK_EXEC_SUCCESS)
        {
            // An error occurred - display a stack trace
            duk_get_prop_string(poContext, -1, "stack");
            printf(duk_safe_to_string(poContext, -1));
        }
        else
        {
            // function executed successfully - get result
            bReturnVal = duk_get_boolean(poContext, -1);
        }
    }
    else
    {
        printf("JS function not found!\n");
        bReturnVal = false;
    }

    duk_pop(poContext); // pop result

    return bReturnVal;
}



