#pragma once
#include <cNetworkConnection.hpp>
#include <Http/HTTP.hpp>
#include <vendor/Utf8.hpp>

class cNetworkServer : public cNetworkConnection
{
    bool pbShutdown = false;

    std::map<string, std::thread> threads;
    std::vector<cNetworkConnection*> aConnections;

    std::function<bool(cNetworkConnection *)> OnConnect = nullptr;
    std::function<bool(cNetworkConnection *)> OnRecieve = nullptr;
    std::function<void(cNetworkConnection *)> OnDisconnect = nullptr;
public:
	cNetworkServer(cNetworkConnection::tNetworkInitializationSettings* ptNetworkSettings) : cNetworkConnection(ptNetworkSettings)
	{
    }

	~cNetworkServer()
    {
        std::cout << "stopping server..." << std::endl;
	    pbShutdown = true;
	    for(auto& [name, t] : threads)
	        t.join();
	    std::cout << "stopped server." << std::endl;
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

    const NET_SOCK oSock = accept(poSock, (struct sockaddr *) &tClientAddr,
                                  &iClientAddrLength);

    if (oSock < 1) return nullptr;

    auto oNewConnection = new cNetworkConnection(oSock, tClientAddr);

    if (pptNetworkSettings->bUseSSL)
    {
        cNetworkAbstractions::SetBlocking(oSock, true);

        oNewConnection->ppConnectionSSL = SSL_new(ppSSLContext);
        SSL_set_fd(oNewConnection->ppConnectionSSL, oSock);

        int iReturn = SSL_accept(oNewConnection->ppConnectionSSL);

        if (iReturn < 0)
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
    threads.insert({"recieve", std::thread(&cNetworkServer::OnRecieveLoop, this)});
    if (pptNetworkSettings->eMode == cNetworkConnection::cMode::eBlocking)
        OnConnectLoop();
    else
        threads.insert({"connect", std::thread(&cNetworkServer::OnConnectLoop, this)});
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
                aConnections.push_back(incoming);
            else
                delete incoming;
        }
        if (!bBlocking) sleep(1);
    }
}

void cNetworkServer::OnRecieveLoop()
{
    while (!pbShutdown)
    {
        for (uint i = 0; i < aConnections.size(); i++)
        {
            auto status = aConnections[i]->Status();
            if (status == cNetworkAbstractions::cConnectionStatus::eAVAILABLE)
            {
                if (OnRecieve)
                    if(!OnRecieve(aConnections[i]))
                    {
                        aConnections.erase(aConnections.begin() + i);
                        i--;
                        continue;
                    }
            }
            else if (status == cNetworkAbstractions::cConnectionStatus::eDISCONNECTED)
            {
                if (OnDisconnect) OnDisconnect(aConnections[i]);
                aConnections.erase(aConnections.begin() + i);
                i--;
                continue;
            }
        }
        sleep(1);
    }
}
