#pragma once
#include <pch.hpp>
#include <iostream>
#include <string>
#include <NetworkAbstractions.hpp>
#include <SslHelper.hpp>

class cNetworkConnection
{
public:
    SSL_CTX* ppSSLContext = nullptr;
    SSL* ppConnectionSSL = nullptr;

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
protected:
	tNetworkInitializationSettings* pptNetworkSettings = nullptr;
	int piLastStatus = 0;
public:
    sockaddr_in ptAddress = {};
    NET_SOCK poSock = NET_INVALID_SOCKET_ID;

	cNetworkConnection(NET_SOCK oSock, sockaddr_in tAddress)
	{
		if (puiNumberOfAliveSockets++ == 0) cNetworkAbstractions::NetInit();
		this->poSock = oSock;
		this->ptAddress = tAddress;
	}

    virtual ~cNetworkConnection()
    {
        if (ppSSLContext) SSL_CTX_free(ppSSLContext);
        Close();
    }

	cNetworkConnection(tNetworkInitializationSettings* ptNetworkSettings);

	void Close();

    bool IsConnected()
    {
        pollfd fdarray = { 0 };
        fdarray.fd = poSock;
        fdarray.events = POLLRDNORM;;
        fdarray.revents = POLLRDNORM;
#ifdef _WIN32
        int poll = WSAPoll(&fdarray, 1, 1000);
#else
        int poll = poll(&fdarray, 1, 1000);
#endif
        if (!(fdarray.revents & POLLRDNORM))
            return false;
        return true;
    }

	// Read and write bytes to the socket stream:
	long ReceiveBytes(byte* pBuffer, size_t uiNumBytes)
    {
        long size = 0;
	    if(ppConnectionSSL)
            size = SSL_read(ppConnectionSSL, (char *) pBuffer, uiNumBytes);
        else
	        size = recv(poSock, (char*)pBuffer, uiNumBytes, 0);
        return size;
    }

	void SendBytes(const byte* pBuffer, size_t uiNumBytes)
    {
        long lResult = 0;
        if (ppConnectionSSL)
            lResult = SSL_write(ppConnectionSSL, pBuffer, uiNumBytes);
        else
            lResult = send(poSock, (char *) pBuffer, uiNumBytes, 0);
        if (lResult == NET_SOCKET_ERROR)
            throw std::runtime_error("Failed to 'send()' bytes! NET_SOCKET_ERROR!");
        else if (static_cast<size_t>(lResult) != uiNumBytes)
            throw std::runtime_error("Failed to 'send()' bytes! Couldn't send all the data!");
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
    // Windows requires a different system call.
    if (ptNetworkSettings->eMode == cMode::eNonBlocking) cNetworkAbstractions::SetBlocking(poSock, false);

    if (ptNetworkSettings->bUseSSL)
    {
        ppSSLContext = cSSLHelper::CreateServerCtx();
        if (ptNetworkSettings->sCertFile.size() == 0 || ptNetworkSettings->sKeyFile.size() == 0)
            throw std::runtime_error("invalid files");
        cSSLHelper::LoadCertificate(ppSSLContext, (char*)ptNetworkSettings->sCertFile.c_str(), (char*)ptNetworkSettings->sKeyFile.c_str());
    }
}

void cNetworkConnection::Close()
{
    if (ppConnectionSSL)
        SSL_free(ppConnectionSSL);
    if (poSock != NET_INVALID_SOCKET_ID)
    {
        if (cNetworkAbstractions::CloseSocket(poSock) != 0)
            throw std::runtime_error("could not close socket");

        if (--puiNumberOfAliveSockets == 0) cNetworkAbstractions::NetShutdown();
    }
}
