#pragma once
#include <pch.hpp>
#include <server/src/ODBC/cODBCInstance.hpp>

cODBCInstance oInstance;
string sUnicodeUTF8 = "おはようございます";

TEST(ODBCTests, Connect)
{
    EXPECT_TRUE(oInstance.Connect("driver=MariaDB ODBC 3.1 Driver;server=192.168.178.187;user=root;pwd=hiddenhand;database=test;"));
}

TEST(ODBCTests, Exe)
{
    EXPECT_TRUE(oInstance.Exec("INSERT INTO UserTest (UserName, Password) VALUES('" + sUnicodeUTF8 + "', 'TEST');"));
}

/*TEST(ODBCTests, Unicode)
{
    std::vector<SQLROW> aUsers;
    EXPECT_TRUE(oInstance.Fetch("SELECT * FROM UserTest ORDER BY Id DESC LIMIT 1", &aUsers));
    EXPECT_TRUE(aUsers.size() == 1);

    string sFetchedUnicodeUTF8;
    EXPECT_TRUE(aUsers[0]["UserName"]->GetValueStr(sFetchedUnicodeUTF8));
    EXPECT_TRUE(sUnicodeUTF8.compare(sFetchedUnicodeUTF8));
}*/

TEST(ODBCTests, Fetch)
{
    std::vector<SQLROW> aUsers;
    EXPECT_TRUE(oInstance.Fetch("SELECT * FROM UserTest", &aUsers));
}

TEST(ODBCTests, DisConnect)
{
    EXPECT_TRUE(oInstance.Disconnect());
}
