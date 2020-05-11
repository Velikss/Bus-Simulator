#pragma once
#include <pch.hpp>
#include <server/src/SSO/SSOHelper.hpp>
#include <server/vendor/StdUuid.hpp>
#include <server/src/SSO/SsoServer.hpp>
#include <server/src/GameServer/CGameServer.hpp>
#include <server/src/cNetworkClient.hpp>

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
    EXPECT_TRUE(sEncoded.compare(
            "UNs2NQkCAADMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzA=="));
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
std::shared_ptr<cNetworkClient> poGameClient;
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
    poGameClient = std::make_shared<cNetworkClient>(ptGameClientSettings.get());

    std::cout << "made shares"<< std::endl;
    EXPECT_TRUE(poSSOServer->Init("driver=MariaDB ODBC 3.1 Driver;server=192.168.178.187;user=root;pwd=hiddenhand;database=test;"));
    EXPECT_TRUE(poSSOServer->Listen());
    std::cout << "made SSO_Server listen"<< std::endl;

    EXPECT_TRUE(poGameServer->Init("driver=MariaDB ODBC 3.1 Driver;server=192.168.178.187;user=game;pwd=game;database=game;"));
    EXPECT_TRUE(poGameServer->Listen());
    std::cout << "made GAME_Server listen"<< std::endl;

    EXPECT_TRUE(poGameClient->Connect());
    std::cout << "made CONNECTED Game client"<< std::endl;
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

TEST(SSOTests, Handshake)
{
    using namespace cHttp;
    cRequest oRequest;
    std::vector<cHeader> aHeaders;

    aHeaders.push_back({"Host", "127.0.0.1"});
    aHeaders.push_back({"Connection", "keep-alive"});

    oRequest.SetMethod(cMethod::ePOST);
    oRequest.SetResource("/player");
    oRequest.SetHeaders(aHeaders);

    string sRequest = oRequest.Serialize();
    EXPECT_TRUE(sRequest.compare("POST /player HTTP/1.1\r\n"
                                 "Host: 127.0.0.1\n\r"
                                 "Connection: keep-alive\r\n"
                                 "\r\n\r\n"));

    poGameClient->SendBytes((const byte*)sRequest.c_str(), sRequest.size());
    sleep(50);
}

TEST(SSOTests, Stop)
{
    poGameClient->Disconnect();
    poGameServer->Stop();
    poSSOServer->Stop();
    EXPECT_TRUE(poGameServer->DestroyDB());
    EXPECT_TRUE(poSSOServer->DestroyDB());
}
