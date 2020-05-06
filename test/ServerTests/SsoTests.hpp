#pragma once
#include <pch.hpp>
#include <server/src/SsoProtocol.hpp>
#include <server/vendor/StdUuid.hpp>

using namespace SSO;

TEST(SSOTests, Hash)
{
    byte aHash[64];
    uint uiHashSize = 0;
    string sMessage = "test123";
    SSO_STATUS iStatus = Blake2Hash((const unsigned char *) (sMessage.c_str()), sMessage.size(),
                                    (unsigned char **) (&aHash), &uiHashSize);
    string sEncoded = base64_encode(aHash, uiHashSize);
    EXPECT_EQ(iStatus, cSSO_OK);
    EXPECT_TRUE(sEncoded.compare("UNs2NQkCAADMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzA=="));
}

cODBCInstance oSSOInstance;
TEST(SSOTests, Login)
{
    EXPECT_TRUE(oSSOInstance.Connect("driver=MariaDB ODBC 3.1 Driver;server=192.168.178.187;user=root;pwd=hiddenhand;database=test;"));
    std::vector<SQLROW> aRows;
    EXPECT_TRUE(oSSOInstance.Fetch("SELECT * FROM User", &aRows));
    EXPECT_TRUE(aRows.size() > 0);
    for(auto& oRow : aRows)
    {
        SQL_TIMESTAMP_STRUCT tStamp{};
        string sTimeStamp;
        oRow["SSOStamp"]->GetValueTimeStamp(tStamp);
        oRow["SSOStamp"]->GetValueStr(sTimeStamp);
        SQL_TIMESTAMP_STRUCT tStamp2 = dt::Now();

        std::cout << "now: " << dt::to_string(tStamp2) << std::endl;
        std::cout << "stamp: " << dt::to_string(tStamp) << std::endl;

        time_t diff = dt::TimeOfTimeStamp(tStamp2) - dt::TimeOfTimeStamp(tStamp);
        std::cout << "diff: " << dt::to_string(dt::TimeToTimeStamp(diff)) << std::endl;
    }
}

TEST(SSOTests, UuidGeneration)
{
    using namespace uuids;
    const uuid oId = uuids::uuid_system_generator{}();
    EXPECT_TRUE(!oId.is_nil());
    string sId = to_string(oId);
    EXPECT_TRUE(sId.size() > 0);
}