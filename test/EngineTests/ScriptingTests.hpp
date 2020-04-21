#pragma once
#include <pch.hpp>
#include <gtest/gtest.h>
#include <scripting/cScriptingEngine.hpp>
#include <scripting/duktape.h>

TEST(ScriptingTests, InitEngine)
{
    cScriptingEngine e = cScriptingEngine();
}