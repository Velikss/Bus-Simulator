#pragma once
#include "NetworkConnection.hpp"

class cNetworkClient : public cNetworkConnection
{
    std::map<string, std::thread> threads;

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

	void Disconnect()
    {
	    if(!pbDestroyed)
        {
            pbShutdown = true;
            for (auto&[name, t] : threads)
                if (t.joinable())
                    t.join();
            CloseConnection();
        }
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

	bool Connect();

private:
    void OnRecieveLoop();
};

bool cNetworkClient::Connect()
{
    cNetworkAbstractions::SetBlocking(poSock, true);

    if (connect(poSock, (const sockaddr *) &ptAddress, sizeof(ptAddress)) != 0)
        return false;

    if (pptNetworkSettings->bUseSSL)
    {
        ppConnectionSSL = SSL_new(ppSSLContext);
        SSL_set_fd(ppConnectionSSL, poSock);

        int iReturn = SSL_connect(ppConnectionSSL);
        if (iReturn <= 0) return false;
    }

    if (pptNetworkSettings->eMode != cNetworkConnection::cMode::eBlocking) cNetworkAbstractions::SetBlocking(poSock, false);

    if(OnConnect) OnConnect(this);
    threads.insert({ "OnRecieveLoop", std::thread(&cNetworkClient::OnRecieveLoop, this) });

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
                    break;
                }
        }
        else if (status == cNetworkAbstractions::cConnectionStatus::eDISCONNECTED)
        {
            if (OnDisconnect) OnDisconnect(this);
            CloseConnection();
            break;
        }
    }
}
