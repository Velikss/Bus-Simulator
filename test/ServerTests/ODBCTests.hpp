#pragma once
#include <pch.hpp>
#include <server/src/ODBC/cODBCInstance.hpp>

std::shared_ptr<cODBCInstance> poInstance;
string sUnicodeUTF8 = "おはようございます";

TEST(ODBCTests, Connect)
{
    poInstance = std::make_shared<cODBCInstance>();
#if defined(WINDOWS)
    EXPECT_TRUE(poInstance->Connect("driver=MariaDB ODBC 3.1 Driver;server=192.168.178.187;user=root;pwd=hiddenhand;database=test-windows;"));
#elif
    EXPECT_TRUE(poInstance->Connect("driver=MariaDB ODBC 3.1 Driver;server=192.168.178.187;user=root;pwd=hiddenhand;database=test-linux;"));
#endif
}

TEST(ODBCTests, Exe)
{
    EXPECT_TRUE(poInstance->Exec("INSERT INTO UserTest (UserName, Password) VALUES('" + sUnicodeUTF8 + "', 'TEST');"));
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
    EXPECT_TRUE(poInstance->Fetch("SELECT * FROM UserTest LIMIT 1", &aUsers));
    EXPECT_TRUE(aUsers.size() == 1);
}

TEST(ODBCTests, SQLInjection)
{
    string sString = "'; DROP TABLE *";
    cODBCInstance::Escape(sString);
    ASSERT_STREQ(sString.c_str(), "\\'; DROP TABLE *");
    EXPECT_TRUE(poInstance->Exec("INSERT INTO UserTest (UserName, Password) VALUES('" + sString + "', 'TEST');"));

    sString = "''; DROP TABLE *";
    cODBCInstance::Escape(sString);
    ASSERT_STREQ(sString.c_str(), "\\'\\'; DROP TABLE *");
    EXPECT_TRUE(poInstance->Exec("INSERT INTO UserTest (UserName, Password) VALUES('" + sString + "', 'TEST');"));
}

TEST(ODBCTests, DisConnect)
{
    EXPECT_TRUE(poInstance->Disconnect());
}
