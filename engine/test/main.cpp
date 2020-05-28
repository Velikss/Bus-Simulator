#include <gtest/gtest.h>
#include "BehaviourTests.hpp"
#include "ScriptingTests.hpp"

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}