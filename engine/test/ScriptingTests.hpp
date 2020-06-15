#pragma once
#include <pch.hpp>
#include <gtest/gtest.h>
#include <scripting/ScriptingEngine.hpp>

TEST(ScriptingTests, TestEngineCompilesWrongFilePath)
{
    cScriptingEngine engine;

    ASSERT_FALSE(engine.CompileJavaScriptFile("/path/to/file"));
}

TEST(ScriptingTests, TestEngineCompilesCorrectFilePath)
{
    cScriptingEngine engine;

    ASSERT_TRUE(engine.CompileJavaScriptFile("test.js"));
}

TEST(ScriptingTests, TestScriptFunction)
{
    cScriptingEngine engine;
    engine.CompileJavaScriptFile("test.js");

    ASSERT_TRUE(engine.RunJavaScriptFunction("test"));
}

TEST(ScriptingTests, TestScriptWrongFunctionName)
{
    cScriptingEngine engine;
    engine.CompileJavaScriptFile("test.js");

    ASSERT_FALSE(engine.RunJavaScriptFunction("testwrong"));
}