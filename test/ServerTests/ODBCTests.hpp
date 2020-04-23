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
}

TEST(ODBCTests, FetchUser)
{
    std::vector<SQLROW> aUsers;
    EXPECT_TRUE(oInstance.QueryExec("SELECT * FROM User", &aUsers));
    for (auto& aRow : aUsers)
    {
        std::cout << "---------------------------" << std::endl;
        SQLINTEGER iValue = 0;
        string sUserName;
        string sPassword;
        aRow["Id"]->GetValueInteger(&iValue);
        aRow["UserName"]->GetValueStr(sUserName);
        aRow["Password"]->GetValueStr(sPassword);
        std::cout << "Id: " << iValue << std::endl;
        std::cout << "UserName: '" << sUserName << "'" << std::endl;
        std::cout << "Password: '" << sPassword << "'" << std::endl;
        std::cout << "---------------------------" << std::endl;
    }
}

TEST(ODBCTests, DisConnect)
{
    EXPECT_TRUE(oInstance.Disconnect());
}
