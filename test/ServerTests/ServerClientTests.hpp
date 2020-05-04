#pragma once
#include <pch.hpp>
#include <gtest/gtest.h>
#include <server/src/Http/HTTP.hpp>
#include <server/src/cNetworkServer.hpp>
#include <server/src/cNetworkClient.hpp>

int iTimesRecieved = 0;

bool OnRecieve(cNetworkConnection* connection)
{
    EXPECT_EQ(connection->GetIP(), "127.0.0.1");
    iTimesRecieved++;
    byte aBytes[5];
    EXPECT_EQ(connection->ReceiveBytes(aBytes, 5), 5);
    const std::string_view sBytes((char*)aBytes, 5);
    EXPECT_TRUE(sBytes.compare("hallo"));
    return false;
}

void OnDisConnect(cNetworkConnection* connection)
{
    EXPECT_EQ(connection->GetIP(), "127.0.0.1");
}

bool OnConnect(cNetworkConnection* connection)
{
    EXPECT_EQ(connection->GetIP(), "127.0.0.1");
    return true;
}

std::shared_ptr<cNetworkServer> pPlainServer = nullptr;
std::shared_ptr<cNetworkClient> pPlainClient = nullptr;
std::shared_ptr<cNetworkConnection::tNetworkInitializationSettings> pPlainServerSettings = nullptr;
std::shared_ptr<cNetworkConnection::tNetworkInitializationSettings> pPlainClientSettings = nullptr;

TEST(ServerClientTests, Bind)
{
    pPlainServerSettings = std::make_shared<cNetworkConnection::tNetworkInitializationSettings>();
    pPlainServerSettings->sAddress = "0.0.0.0";
    pPlainServerSettings->usPort = 81;
    pPlainServerSettings->eIPVersion = cNetworkConnection::cIPVersion::eV4;
    pPlainServerSettings->eConnectionType = cNetworkConnection::cConnectionType::eTCP;
    pPlainServerSettings->eMode = cNetworkConnection::cMode::eNonBlocking;

    pPlainServer = std::make_shared<cNetworkServer>(pPlainServerSettings.get());
    pPlainServer->SetOnConnectEvent(OnConnect);
    pPlainServer->SetOnRecieveEvent(OnRecieve);
    pPlainServer->SetOnDisconnectEvent(OnDisConnect);
    EXPECT_TRUE(pPlainServer->Listen());
}

TEST(ServerClientTests, Connect)
{
    pPlainClientSettings = std::make_shared<cNetworkConnection::tNetworkInitializationSettings>();
    pPlainClientSettings->sAddress = "0.0.0.0";
    pPlainClientSettings->usPort = 81;
    pPlainClientSettings->eIPVersion = cNetworkConnection::cIPVersion::eV4;
    pPlainClientSettings->eConnectionType = cNetworkConnection::cConnectionType::eTCP;
    pPlainClientSettings->eMode = cNetworkConnection::cMode::eNonBlocking;

    pPlainClient = std::make_shared<cNetworkClient>(pPlainClientSettings.get());
    EXPECT_TRUE(pPlainClient->Connect());
    string sGreeting = "Hallo";
    pPlainClient->SendBytes((byte*)sGreeting.c_str(), sGreeting.size());
    sleep(50);
    EXPECT_EQ(iTimesRecieved, 1);
    pPlainClient->Disconnect();
    pPlainServer->Stop();
}

std::shared_ptr<cNetworkServer> pSSLServer = nullptr;
std::shared_ptr<cNetworkClient> pSSLClient = nullptr;
std::shared_ptr<cNetworkConnection::tNetworkInitializationSettings> pSSLServerSettings = nullptr;
std::shared_ptr<cNetworkConnection::tNetworkInitializationSettings> pSSLClientSettings = nullptr;

TEST(ServerClientTests, BindSSL)
{
    pSSLServerSettings = std::make_shared<cNetworkConnection::tNetworkInitializationSettings>();
    pSSLServerSettings->sAddress = "0.0.0.0";
    pSSLServerSettings->usPort = 82;
    pSSLServerSettings->bUseSSL = true;
    pSSLServerSettings->sCertFile = "./19-4-2020.cert";
    pSSLServerSettings->sKeyFile = "./19-4-2020.key";
    pSSLServerSettings->eIPVersion = cNetworkConnection::cIPVersion::eV4;
    pSSLServerSettings->eConnectionType = cNetworkConnection::cConnectionType::eTCP;
    pSSLServerSettings->eMode = cNetworkConnection::cMode::eNonBlocking;

    pSSLServer = std::make_shared<cNetworkServer>(pSSLServerSettings.get());
    pSSLServer->SetOnConnectEvent(OnConnect);
    pSSLServer->SetOnRecieveEvent(OnRecieve);
    pSSLServer->SetOnDisconnectEvent(OnDisConnect);
    EXPECT_TRUE(pSSLServer->Listen());
}

TEST(ServerClientTests, ConnectSSL)
{
    pSSLClientSettings = std::make_shared<cNetworkConnection::tNetworkInitializationSettings>();
    pSSLClientSettings->sAddress = "0.0.0.0";
    pSSLClientSettings->usPort = 82;
    pSSLClientSettings->bUseSSL = true;
    pSSLClientSettings->eIPVersion = cNetworkConnection::cIPVersion::eV4;
    pSSLClientSettings->eConnectionType = cNetworkConnection::cConnectionType::eTCP;
    pSSLClientSettings->eMode = cNetworkConnection::cMode::eNonBlocking;

    pSSLClient = std::make_shared<cNetworkClient>(pSSLClientSettings.get());
    EXPECT_TRUE(pSSLClient->Connect());
    string sGreeting = "Hallo";
    pSSLClient->SendBytes((byte*)sGreeting.c_str(), sGreeting.size());
    sleep(50);
    EXPECT_EQ(iTimesRecieved, 2);
    pSSLClient->Disconnect();
    pSSLServer->Stop();
}
