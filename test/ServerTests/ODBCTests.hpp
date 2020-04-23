#pragma once
#include <pch.hpp>
#include <server/src/ODBC/cODBCInstance.hpp>

cODBCInstance oInstance;

TEST(ODBCTests, Connect)
{
    EXPECT_TRUE(oInstance.Connect("driver=MariaDB ODBC 3.1 Driver;server=192.168.178.187;user=root;pwd=hiddenhand;database=test;"));
}

TEST(ODBCTests, Exe)
{
    EXPECT_TRUE(oInstance.Exec("INSERT INTO UserTest (UserName, Password) VALUES('TEST', 'TEST');"));
}

TEST(ODBCTests, Fetch)
{
    std::vector<SQLROW> aUsers;
    EXPECT_TRUE(oInstance.Fetch("SELECT * FROM UserTest", &aUsers));
}

TEST(ODBCTests, DisConnect)
{
    EXPECT_TRUE(oInstance.Disconnect());
}
