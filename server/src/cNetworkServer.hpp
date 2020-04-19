#pragma once
#include <cNetworkConnection.hpp>

class cNetworkServer : public cNetworkConnection
{
public:
	cNetworkServer(cNetworkConnection::tNetworkInitializationSettings* ptNetworkSettings) : cNetworkConnection(ptNetworkSettings)
	{
	}

	bool Listen();

	cNetworkConnection* AcceptConnection(bool bBlockingSocket = false) const;
};

bool cNetworkServer::Listen()
{
    if ((piLastStatus = bind(poSock, (const sockaddr*)&ptAddress, sizeof(ptAddress))) != 0)
    {
        Close();
        return false;
    }
    if ((piLastStatus = listen(poSock, SOMAXCONN)) != 0)
    {
        Close();
        return false;
    }
    return true;
}

cNetworkConnection *cNetworkServer::AcceptConnection(bool bBlockingSocket) const
{
    sockaddr_in tClientAddr = {};
    socklen_t iClientAddrLength = sizeof(tClientAddr);
    const NET_SOCK oSock = accept(poSock, (struct sockaddr *) &tClientAddr,
                                  &iClientAddrLength);

    if (oSock == -1) return nullptr;

    auto oNewConnection = new cNetworkConnection(oSock, tClientAddr);

    if (pptNetworkSettings->bUseSSL)
    {
        oNewConnection->ppConnectionSSL = SSL_new(ppSSLContext);
        SSL_set_fd(oNewConnection->ppConnectionSSL, oSock);

        int iReturn = SSL_accept(oNewConnection->ppConnectionSSL);
        if (iReturn < 0) return nullptr;
    }

    if (!bBlockingSocket) cNetworkAbstractions::SetBlocking(oSock, bBlockingSocket);

    return oNewConnection;
}
