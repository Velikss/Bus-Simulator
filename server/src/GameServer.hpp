#pragma once
#include <pch.hpp>
#include <NetworkServer.hpp>

bool RecieveData(cNetworkConnection* pConnection, byte* & buffer, int & iRecievedContent)
{
    int iSize = 0;
    const size_t recievedSize = (size_t) pConnection->ReceiveBytes(((byte *) &iSize), 4); //-V206 //-V112
    if (recievedSize != 4) std::cout << "didn't recieve header." << std::endl; //-V112
    buffer = new byte[iSize]; //-V121
    while (iRecievedContent != iSize)
    {
        iRecievedContent += pConnection->ReceiveBytes(buffer + iRecievedContent, iSize - iRecievedContent); //-V104
        if (iRecievedContent == -1) iRecievedContent += 1;
    }
    return true;
}

bool SendData(cNetworkConnection* pConnection, byte* buffer, int iSize)
{
    if(!pConnection->SendBytes((byte*)&iSize, 4)) return false; //-V206 //-V112
    if(!pConnection->SendBytes(buffer, iSize)) return false;
    return true;
}

class cGameServer : protected cNetworkServer
{
public:
    cGameServer(cNetworkConnection::tNetworkInitializationSettings* tSettings) : cNetworkServer(tSettings)
    {
        std::function<bool(cNetworkConnection*)> _OnConnect = std::bind(&cGameServer::OnConnect, this, std::placeholders::_1);
        std::function<bool(cNetworkConnection*)> _OnRecieve = std::bind(&cGameServer::OnRecieve, this, std::placeholders::_1);
        std::function<void(cNetworkConnection*)> _OnDisconnect = std::bind(&cGameServer::OnDisconnect, this, std::placeholders::_1);
        SetOnConnectEvent(_OnConnect);
        SetOnRecieveEvent(_OnRecieve);
        SetOnDisconnectEvent(_OnDisconnect);
    }

public:
    bool Listen()
    {
        return cNetworkServer::Listen();
    }

    void Stop()
    {
        cNetworkServer::Stop();
    }

    bool OnConnect(cNetworkConnection* pConnection);
    bool OnRecieve(cNetworkConnection* pConnection);
    void OnDisconnect(cNetworkConnection* pConnection);
};

bool cGameServer::OnConnect(cNetworkConnection* pConnection)
{
    std::cout << "New connection from " << pConnection->GetConnectionString() << std::endl;
    return true;
}

bool cGameServer::OnRecieve(cNetworkConnection *pConnection)
{
    byte* buffer = nullptr;
    int iRecievedContent = 0;
    if (!RecieveData(pConnection, buffer, iRecievedContent))
        return false;
 //-V808
    glm::vec3* oPos = (glm::vec3*)&buffer[36];
    glm::vec3* oRot = (glm::vec3*) & buffer[36 + sizeof(glm::vec3)];
    for(uint i = 0; i < paConnections.size(); i++) //-V104
        if (paConnections[i] != pConnection) //-V108
        {
            if (!SendData(paConnections[i], buffer, iRecievedContent)) //-V108
            {
                std::cout << "failed sending to " << paConnections[i]->GetConnectionString() << ", error: " << paConnections[i]->piFailures << std::endl;; //-V108
                if (paConnections[i]->piFailures++ >= 5) //-V108
                {
                    std::cout << "terminated connection " << paConnections[i]->GetConnectionString() << std::endl;; //-V108
                    RemoveConnectionAt(i);
                }
            }
        }
    delete buffer;
    return true;
}

void cGameServer::OnDisconnect(cNetworkConnection *pConnection)
{
    std::cout << pConnection->GetConnectionString() << ", disconnected." << std::endl;
}
