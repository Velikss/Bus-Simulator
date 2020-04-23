#pragma once
#include <pch.hpp>
#include <server/src/ODBC/cODBCInstance.hpp>

cODBCInstance oInstance;

TEST(ODBCTests, Connect)
{
    EXPECT_TRUE(oInstance.Connect("driver=MariaDB ODBC 3.1 Driver;server=192.168.178.187;user=root;pwd=hiddenhand;database=test;"));
}

TEST(ODBCTests, Discover)
{
    EXPECT_TRUE(oInstance.Discover());
    oInstance.PrintTables();
}

TEST(ODBCTests, FetchUser)
{
    std::vector<tUser> aUsers;
    EXPECT_TRUE(oInstance.QueryExec("SELECT Id FROM User", &aUsers));
}

TEST(ODBCTests, DisConnect)
{
    EXPECT_TRUE(oInstance.Disconnect());
}