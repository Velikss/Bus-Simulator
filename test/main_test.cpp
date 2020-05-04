#include <pch.hpp>
#include <gtest/gtest.h>
#include <ServerTests/UriTests.hpp>
#include <ServerTests/ODBCTests.hpp>
#ifndef JENKINS
#include <ServerTests/ServerClientTests.hpp>
#endif
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}