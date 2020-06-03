#pragma once
#include "NetworkConnection.hpp"
#include "Http/HTTP.hpp"
#include "../vendor/Utf8.hpp"

class cNetworkServer : public cNetworkConnection
{
protected:
    bool pbSleepOnRecieveOverride = false;
    bool pbSleepOnConnectOverride = false;

    std::map<string, std::thread> paThreads;
    std::vector<cNetworkConnection*> paConnections;

    std::function<bool(cNetworkConnection *)> OnConnect = nullptr;
    std::function<bool(cNetworkConnection *)> OnRecieve = nullptr;
    std::function<void(cNetworkConnection *)> OnDisconnect = nullptr;

    void RemoveConnectionAt(uint& i);
public:
	cNetworkServer(cNetworkConnection::tNetworkInitializationSettings* ptNetworkSettings) : cNetworkConnection(ptNetworkSettings)
	{
        if (ptNetworkSettings->bUseSSL)
        {
            ppSSLContext = cSSLHelper::CreateServerCtx();
            if (ptNetworkSettings->sCertFile.size() > 0 && ptNetworkSettings->sKeyFile.size() > 0)
                if(!cSSLHelper::LoadCertificate(ppSSLContext, ptNetworkSettings->sCertFile, ptNetworkSettings->sKeyFile))
                {
                    std::cout << "could not find certificates." << std::endl;
                    throw std::runtime_error("could not find certificates.");
                }
        }
    }

    void Stop()
    {
        pbShutdown = true;
        for (auto&[name, t] : paThreads)
            if (t.joinable())
                t.join();
        CloseConnection();
        paThreads.clear();
    }

	~cNetworkServer()
    {
	    Stop();
    }

	void SetOnConnectEvent(const std::function<bool(cNetworkConnection *)> &OnConnect)
    {
	    this->OnConnect = OnConnect;
    }

    void SetOnRecieveEvent(const std::function<bool(cNetworkConnection *)> &OnRecieve)
    {
        this->OnRecieve = OnRecieve;
    }

    void SetOnDisconnectEvent(const std::function<void(cNetworkConnection *)> &OnDisconnect)
    {
        this->OnDisconnect = OnDisconnect;
    }

	bool Listen();
private:
    void OnConnectLoop();
    void OnRecieveLoop();
	cNetworkConnection* AcceptConnection(bool bBlockingSocket = false) const;
};

cNetworkConnection *cNetworkServer::AcceptConnection(bool bBlockingSocket) const
{
    sockaddr_in tClientAddr = {};
    socklen_t iClientAddrLength = sizeof(tClientAddr);

    const NET_SOCK oSock = (NET_SOCK) accept(poSock, (struct sockaddr *) &tClientAddr, //-V106
                                  &iClientAddrLength);

    if (oSock < 1) return nullptr;

    auto oNewConnection = new cNetworkConnection(oSock, tClientAddr, bBlockingSocket);

    if (pptNetworkSettings->bUseSSL)
    {
        cNetworkAbstractions::SetBlocking(oSock, true);

        oNewConnection->ppConnectionSSL = SSL_new(ppSSLContext);
        SSL_set_fd(oNewConnection->ppConnectionSSL, oSock);

        int iReturn = SSL_accept(oNewConnection->ppConnectionSSL);
        if (iReturn <= 0)
        {
            delete oNewConnection;
            return nullptr;
        }
    }

    if (!bBlockingSocket) cNetworkAbstractions::SetBlocking(oSock, bBlockingSocket);

    return oNewConnection;
}

bool cNetworkServer::Listen()
{
    if ((piLastStatus = bind(poSock, (const sockaddr*)&ptAddress, sizeof(ptAddress))) != 0) //-V106
    {
        CloseConnection();
        return false;
    }
    if ((piLastStatus = listen(poSock, SOMAXCONN)) != 0) //-V106
    {
        CloseConnection();
        return false;
    }
    paThreads.insert({"recieve", std::thread(&cNetworkServer::OnRecieveLoop, this)});
    if (pptNetworkSettings->eMode == cNetworkConnection::cMode::eBlocking)
        OnConnectLoop();
    else
        paThreads.insert({"connect", std::thread(&cNetworkServer::OnConnectLoop, this)});
    return true;
}

void cNetworkServer::OnConnectLoop()
{
    const bool bBlocking = pptNetworkSettings->eMode == cNetworkConnection::cMode::eBlocking;
    while (!pbShutdown)
    {
        cNetworkConnection *incoming;
        if ((incoming = AcceptConnection(bBlocking)) != nullptr)
        {
            bool bPass = true;
            if(OnConnect) bPass = OnConnect(incoming);
            if(bPass)
                paConnections.push_back(incoming);
            else
                delete incoming;
        }
        if (!bBlocking && !pbSleepOnConnectOverride) fSleep(1);
    }
}

void cNetworkServer::OnRecieveLoop()
{
    while (!pbShutdown) //-V104
    {
        for (uint i = 0; i < (uint)paConnections.size(); i++)
        {
            auto status = paConnections[i]->Status(); //-V108
            if (status == cNetworkAbstractions::cConnectionStatus::eAVAILABLE)
            {
                if (OnRecieve && !paConnections[i]->IsRecieveLocked()) //-V108
                    if(!OnRecieve(paConnections[i])) //-V108
                    {
                        paConnections.erase(paConnections.begin() + i);
                        i--;
                        continue;
                    }
            }
            else if (status == cNetworkAbstractions::cConnectionStatus::eDISCONNECTED)
            {
                RemoveConnectionAt(i);
                continue;
            }
        }
        if(!pbSleepOnRecieveOverride) fSleep(1);
    }
}

void cNetworkServer::RemoveConnectionAt(uint& i)
{
    if (OnDisconnect) OnDisconnect(paConnections[i]); //-V108
    paConnections.erase(paConnections.begin() + i);
    i--;
}
