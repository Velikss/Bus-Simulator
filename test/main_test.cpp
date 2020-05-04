#include <gtest/gtest.h>
#include <EngineTests/BehaviourTests.hpp>
#include <EngineTests/ScriptingTests.hpp>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}