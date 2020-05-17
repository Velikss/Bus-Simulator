#pragma once
#include <pch.hpp>
#include <iostream>
#include <string>
#include "NetworkAbstractions.hpp"
#include "SslHelper.hpp"

class cNetworkConnection
{
public:
    enum class cMode { eBlocking, eNonBlocking };
    enum class cIPVersion { eV4, eV6 };
    enum class cConnectionType { eTCP, eUDP };
    struct tNetworkInitializationSettings
    {
        cMode eMode = cMode::eBlocking;
        cIPVersion eIPVersion = cIPVersion::eV4;
        cConnectionType eConnectionType = cConnectionType::eTCP;
        std::string sAddress = "127.0.0.1";
        unsigned short usPort = 0;
        bool bUseSSL = false;
        std::string sCertFile = "";
        std::string sKeyFile = "";
    };

    int piFailures = 0;
protected:
    bool pbLockedRecieve = false;
    bool pbShutdown = false;
    bool pbDestroyed = false;
    bool pbBlocking = false;

    tNetworkInitializationSettings* pptNetworkSettings = nullptr;
    int piLastStatus = 0;
    sockaddr_in ptAddress = {};
    NET_SOCK poSock = NET_INVALID_SOCKET_ID;
public:
    SSL_CTX* ppSSLContext = nullptr;
    SSL* ppConnectionSSL = nullptr;
	cNetworkConnection(NET_SOCK oSock, sockaddr_in tAddress, bool bBlocking)
	{
		if (puiNumberOfAliveSockets++ == 0) cNetworkAbstractions::NetInit();
		this->poSock = oSock;
		this->ptAddress = tAddress;
		this->pbBlocking = bBlocking;
	}

    virtual ~cNetworkConnection()
    {
        CloseConnection();
    }

	cNetworkConnection(tNetworkInitializationSettings* ptNetworkSettings);

	void CloseConnection();

    bool IsConnected()
    {
	    return (cNetworkAbstractions::IsConnected(poSock, pptNetworkSettings->eMode == cMode::eBlocking) == cNetworkAbstractions::cConnectionStatus::eCONNECTED);
    }

    bool Available()
    {
        return (cNetworkAbstractions::IsConnected(poSock, pbBlocking) == cNetworkAbstractions::cConnectionStatus::eAVAILABLE);
    }

    cNetworkAbstractions::cConnectionStatus Status()
    {
        return cNetworkAbstractions::IsConnected(poSock, pbBlocking);
    }

    void LockRecieve()
    {
        pbLockedRecieve = true;
    }

    void UnLockRecieve()
    {
        pbLockedRecieve = false;
    }

    bool IsRecieveLocked()
    {
        return pbLockedRecieve;
    }

    string GetIP()
    {
        return string(inet_ntoa(ptAddress.sin_addr));
    }

    ushort GetPort()
    {
        return ptAddress.sin_port;
    }

    string GetPortStr()
    {
	    return std::to_string(GetPort());
    }

    string GetConnectionString()
    {
	    return GetIP() + ":" + GetPortStr();
    }

	bool SendBytes(const byte* pBuffer, size_t uiNumBytes)
    {
        long long lResult = 0;
        if (ppConnectionSSL)
            lResult = SSL_write(ppConnectionSSL, pBuffer, uiNumBytes);
        else
            lResult = send(poSock, (char *) pBuffer, uiNumBytes, 0);
        if (lResult == NET_SOCKET_ERROR) return false;
        return true;
    }

    long ReceiveBytes(byte* pBuffer, size_t uiNumBytes)
    {
        long long size = 0;
        if(ppConnectionSSL)
            size = SSL_read(ppConnectionSSL, (char*) pBuffer, uiNumBytes);
        else
            size = recv(poSock, (char*)pBuffer, uiNumBytes, 0);
        return size;
    }

	static unsigned int puiNumberOfAliveSockets;
};
unsigned int cNetworkConnection::puiNumberOfAliveSockets = 0;

cNetworkConnection::cNetworkConnection(cNetworkConnection::tNetworkInitializationSettings *ptNetworkSettings)
{
    if (puiNumberOfAliveSockets++ == 0) cNetworkAbstractions::NetInit();
    this->pptNetworkSettings = ptNetworkSettings;

    ptAddress.sin_family = ptNetworkSettings->eIPVersion == cIPVersion::eV4 ? AF_INET : AF_INET6;
    ptAddress.sin_addr.s_addr = (ptNetworkSettings->sAddress.empty()) ? INADDR_ANY : inet_addr(ptNetworkSettings->sAddress.c_str());
    ptAddress.sin_port = htons(ptNetworkSettings->usPort);

    poSock = socket(ptNetworkSettings->eIPVersion == cIPVersion::eV4 ? AF_INET : AF_INET6,
                    ptNetworkSettings->eConnectionType == cConnectionType::eTCP ? SOCK_STREAM : SOCK_DGRAM,
                    IPPROTO_TCP);

    if (poSock == -1 || poSock == NET_INVALID_SOCKET_ID)
        throw std::runtime_error("invalid");

    // Optionally set non-blocking.
    if (ptNetworkSettings->eMode == cMode::eNonBlocking) cNetworkAbstractions::SetBlocking(poSock, false);
}

void cNetworkConnection::CloseConnection()
{
    if (!pbDestroyed)
    {
        if (poSock != NET_INVALID_SOCKET_ID)
        {
            if (cNetworkAbstractions::CloseSocket(poSock) != 0)
                throw std::runtime_error("could not close socket");

            if (--puiNumberOfAliveSockets == 0) cNetworkAbstractions::NetShutdown();
        }
        if (ppConnectionSSL)
            SSL_free(ppConnectionSSL);
        if (ppSSLContext)
            SSL_CTX_free(ppSSLContext);
    }
    pbDestroyed = true;
}
