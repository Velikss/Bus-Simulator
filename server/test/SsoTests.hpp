#pragma once
#include <pch.hpp>
#include "../src/SSO/SsoHelper.hpp"
#include "../vendor/StdUuid.hpp"
#include "../src/SSO/SsoServer.hpp"
#include "../src/GameServer/GameServer.hpp"
#include "../src/SSO/SsoClient.hpp"
#include <filesystem>

using namespace SSO;
using namespace cHttp;

TEST(SSOTests, Hash)
{
    byte *aHash = new byte[128];
    uint uiHashSize = 0;
    string sMessage = "test123";
    SSO_STATUS iStatus = Blake2Hash((const unsigned char *) (sMessage.c_str()), sMessage.size(),
                                    (unsigned char **) (&aHash), &uiHashSize);
    string sEncoded = base64_encode(aHash, uiHashSize);
    EXPECT_EQ(iStatus, C_SSO_OK);
    if (Is64Bit())
        EXPECT_STREQ(sEncoded.c_str(), "/emPKvUfnQlOF+7T2aWeaWHxXbxsMDBkA+/btWGE8H85QDtkE3TAl1h4CC3HT2zBaNPRT52zxqzEUMLkxMqrYQ==");
    else
        EXPECT_STREQ(sEncoded.c_str(), "7HAHoQ7YwnORLBBqBnxo14ojo+UQWh8xb0hb9dZ0LMU=");
}

TEST(SSOTests, UuidGeneration)
{
    using namespace uuids;
    const uuid oId = uuids::uuid_system_generator{}();
    EXPECT_TRUE(!oId.is_nil());
    string sId = to_string(oId);
    std::cout << sId << std::endl;
    EXPECT_TRUE(sId.size() > 0);
}

std::shared_ptr<cSSOServer> poSSOServer;
std::shared_ptr<cGameServer> poGameServer;
std::shared_ptr<cSSOClient> poGameClient;
std::shared_ptr<cNetworkConnection::tNetworkInitializationSettings> ptSSOServerSettings = nullptr;
std::shared_ptr<cNetworkConnection::tNetworkInitializationSettings> ptGameServerSettings = nullptr;
std::shared_ptr<cNetworkConnection::tNetworkInitializationSettings> ptGameClientSettings = nullptr;

TEST(SSOTests, SetupSSOServer)
{
    ptSSOServerSettings = std::make_shared<cNetworkConnection::tNetworkInitializationSettings>();
    ptSSOServerSettings->sAddress = "127.0.0.1";
    ptSSOServerSettings->usPort = 14001;
    ptSSOServerSettings->eIPVersion = cNetworkConnection::cIPVersion::eV4;
    ptSSOServerSettings->eConnectionType = cNetworkConnection::cConnectionType::eTCP;
    ptSSOServerSettings->eMode = cNetworkConnection::cMode::eNonBlocking;

    ptGameServerSettings = std::make_shared<cNetworkConnection::tNetworkInitializationSettings>();
    ptGameServerSettings->sAddress = "127.0.0.1";
    ptGameServerSettings->usPort = 14000;
    ptGameServerSettings->eIPVersion = cNetworkConnection::cIPVersion::eV4;
    ptGameServerSettings->eConnectionType = cNetworkConnection::cConnectionType::eTCP;
    ptGameServerSettings->eMode = cNetworkConnection::cMode::eNonBlocking;

    ptGameClientSettings = std::make_shared<cNetworkConnection::tNetworkInitializationSettings>();
    ptGameClientSettings->sAddress = "127.0.0.1";
    ptGameClientSettings->usPort = 14000;
    ptGameClientSettings->eIPVersion = cNetworkConnection::cIPVersion::eV4;
    ptGameClientSettings->eConnectionType = cNetworkConnection::cConnectionType::eTCP;
    ptGameClientSettings->eMode = cNetworkConnection::cMode::eNonBlocking;

    poSSOServer = std::make_shared<cSSOServer>(ptSSOServerSettings.get());
    poGameServer = std::make_shared<cGameServer>(ptGameServerSettings.get());
    poGameClient = std::make_shared<cSSOClient>(ptGameClientSettings.get());

    string sGameServerUuid = uuids::to_string(uuids::uuid_system_generator{}());

#if defined(WINDOWS)
    EXPECT_TRUE(poSSOServer->InitDB("driver=MariaDB ODBC 3.1 Driver;server=192.168.178.187;user=root;pwd=hiddenhand;database=test-windows;"));
#else
    EXPECT_TRUE(poSSOServer->InitDB("driver=MariaDB ODBC 3.1 Driver;server=192.168.178.187;user=root;pwd=hiddenhand;database=test-linux;"));
#endif

    poSSOServer->RegisterUuid(sGameServerUuid);
    EXPECT_TRUE(poSSOServer->Listen());

#if defined(WINDOWS)
    EXPECT_TRUE(poGameServer->InitDB("driver=MariaDB ODBC 3.1 Driver;server=192.168.178.187;user=root;pwd=hiddenhand;database=test-windows;"));
#else
    EXPECT_TRUE(poGameServer->InitDB("driver=MariaDB ODBC 3.1 Driver;server=192.168.178.187;user=root;pwd=hiddenhand;database=test-linux;"));
#endif

    EXPECT_TRUE(poGameServer->ConnectToSSOServer(sGameServerUuid, "127.0.0.1", 14001));
    EXPECT_TRUE(poGameServer->Listen());

    EXPECT_TRUE(poGameClient->Connect());
//    EXPECT_TRUE(oSSOInstance.Connect("driver=MariaDB ODBC 3.1 Driver;server=192.168.178.187;user=root;pwd=hiddenhand;database=test;"));
//    std::vector<SQLROW> aRows;
//    EXPECT_TRUE(oSSOInstance.Fetch("SELECT * FROM User", &aRows));
//    EXPECT_TRUE(aRows.size() > 0);
//    for(auto& oRow : aRows)
//    {
//        SQL_TIMESTAMP_STRUCT tStamp{};
//        string sTimeStamp;
//        oRow["SSOStamp"]->GetValueTimeStamp(tStamp);
//        oRow["SSOStamp"]->GetValueStr(sTimeStamp);
//        SQL_TIMESTAMP_STRUCT tStamp2 = dt::Now();
//
//        std::cout << "now: " << dt::to_string(tStamp2) << std::endl;
//        std::cout << "stamp: " << dt::to_string(tStamp) << std::endl;
//
//        time_t diff = dt::TimeOfTimeStamp(tStamp2) - dt::TimeOfTimeStamp(tStamp);
//        std::cout << "diff: " << dt::to_string(dt::TimeToTimeStamp(diff)) << std::endl;
//    }
}

TEST(SSOTests, CreateUserOnSSO)
{
    EXPECT_TRUE(poSSOServer->CreateUser("root", "password"));
}

TEST(SSOTests, RequestResource)
{
    // request resource.
    cRequest oRequest;
    cResponse oResponse;

    std::vector<cHeader> aHeaders;

    aHeaders.push_back({"Host", "127.0.0.1"});
    aHeaders.push_back({"Connection", "keep-alive"});
    aHeaders.push_back({"session-key", "abcd"});

    oRequest.SetMethod(cMethod::ePOST);
    oRequest.SetResource("/player");
    oRequest.SetHeaders(aHeaders);

    EXPECT_TRUE(poGameClient->SendRequest(oRequest, oResponse));
    EXPECT_EQ(403, oResponse.GetResponseCode());
    EXPECT_STREQ("login", oResponse.GetHeader("order").c_str());
}

TEST(SSOTests, Login)
{
    EXPECT_TRUE(poGameClient->Login("root", "password"));
}

TEST(SSOTests, FinishRequest)
{
    cRequest oRequest;
    cResponse oResponse;

    std::vector<cHeader> aHeaders;

    aHeaders.push_back({"Host", "127.0.0.1"});
    aHeaders.push_back({"Connection", "keep-alive"});

    oRequest.SetMethod(cMethod::ePOST);
    oRequest.SetResource("/player");
    oRequest.SetHeaders(aHeaders);

    EXPECT_TRUE(poGameClient->SendRequest(oRequest, oResponse));
    EXPECT_EQ(oResponse.GetResponseCode(), 200);
}

TEST(SSOTests, Logout)
{
    EXPECT_TRUE(poGameClient->Logout());
}

TEST(SSOTests, Stop)
{
    poGameClient->Disconnect();
    poGameServer->Stop();
    poSSOServer->Stop();
    EXPECT_TRUE(poGameServer->DestroyDB());
    EXPECT_TRUE(poSSOServer->DestroyDB());
}
