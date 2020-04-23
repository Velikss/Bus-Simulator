#pragma once
#include <pch.hpp>
#include <server/src/ODBC/cODBCInstance.hpp>

cODBCInstance oInstance;

TEST(ODBCTests, Connect)
{
    EXPECT_TRUE(oInstance.Connect());
}

TEST(ODBCTests, DisConnect)
{
    EXPECT_TRUE(oInstance.Disconnect());
}