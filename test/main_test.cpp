#include <gtest/gtest.h>
#include <ServerTests/UriTests.hpp>
#include <ServerTests/ODBCTests.hpp>
#include <ServerTests/ServerClientTests.hpp>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}