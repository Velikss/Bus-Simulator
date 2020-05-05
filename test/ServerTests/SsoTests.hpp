#pragma once
#include <pch.hpp>
#include <server/src/SsoProtocol.hpp>

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