#pragma once
#include <pch.hpp>
#include <server/src/ODBC/cODBCInstance.hpp>

cODBCInstance oInstance;

TEST(ODBCTests, Connect)
{
    EXPECT_TRUE(oInstance.Connect("driver=MariaDB ODBC 3.1 Driver;server=192.168.178.187;user=root;pwd=hiddenhand;database=test;"));
}

TEST(ODBCTests, DisConnect)
{
    EXPECT_TRUE(oInstance.Disconnect());
}