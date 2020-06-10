#pragma once
#include "NetworkConnection.hpp"

class cNetworkClient : public cNetworkConnection
{
    std::map<string, std::thread> paThreads;

    std::function<void(cNetworkConnection *)> OnConnect = nullptr;
    std::function<bool(cNetworkConnection *)> OnRecieve = nullptr;
    std::function<void(cNetworkConnection *)> OnDisconnect = nullptr;
public:
	cNetworkClient(cNetworkConnection::tNetworkInitializationSettings* ptNetworkSettings) : cNetworkConnection(ptNetworkSettings)
	{
        if (ptNetworkSettings->bUseSSL)
        {
            ppSSLContext = cSSLHelper::CreateClientCtx();
            if (ptNetworkSettings->sCertFile.size() > 0 && ptNetworkSettings->sKeyFile.size() > 0)
                cSSLHelper::LoadCertificate(ppSSLContext, (char*)ptNetworkSettings->sCertFile.c_str(), (char*)ptNetworkSettings->sKeyFile.c_str());
        }
	}

	virtual void Disconnect()
    {
        pbShutdown = true;
        for (auto&[name, t] : paThreads)
            if (t.joinable())
                t.join();
        CloseConnection();
        paThreads.clear();
    }

    ~cNetworkClient()
    {
	    Disconnect();
    }

    void SetOnConnectEvent(const std::function<void(cNetworkConnection *)> &OnConnect)
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

	virtual bool Connect(long lTimeOut = 5);

private:
    virtual void OnRecieveLoop();
};

bool cNetworkClient::Connect(long lTimeOut)
{
    cNetworkAbstractions::SetBlocking(poSock, false);

    connect(poSock, (const sockaddr *) &ptAddress, sizeof(ptAddress));
    fd_set fdset;
    struct timeval tv;
    FD_ZERO(&fdset);
    FD_SET(poSock, &fdset);
    tv.tv_sec = lTimeOut;             /* 10 second timeout */
    tv.tv_usec = 0;

    if (select(poSock + 1, NULL, &fdset, NULL, &tv) == 1)
    {
        if (cNetworkAbstractions::GetLastError(poSock) != 0)
            return false;
    }
    else
        return false;

    if (pptNetworkSettings->bUseSSL)
    {
        cNetworkAbstractions::SetBlocking(poSock, true);

        ppConnectionSSL = SSL_new(ppSSLContext);
        SSL_set_fd(ppConnectionSSL, poSock);

        int iReturn = SSL_connect(ppConnectionSSL);
        if (iReturn <= 0) return false;
    }

    cNetworkAbstractions::SetBlocking(poSock, (pptNetworkSettings->eMode == cNetworkConnection::cMode::eBlocking));

    if(OnConnect) OnConnect(this);
    paThreads.insert({"OnRecieveLoop", std::thread(&cNetworkClient::OnRecieveLoop, this) });

    return true;
}

void cNetworkClient::OnRecieveLoop()
{
    while (!pbShutdown)
    {
        auto status = Status();
        if (status == cNetworkAbstractions::cConnectionStatus::eAVAILABLE)
        {
            if (OnRecieve)
                if (!OnRecieve(this))
                {
                    CloseConnection();
                    return;
                }
        }
        else if (status == cNetworkAbstractions::cConnectionStatus::eDISCONNECTED)
        {
            if (OnDisconnect) OnDisconnect(this);
            CloseConnection();
            return;
        }
    }
}
