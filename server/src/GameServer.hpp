#pragma once
#include <pch.hpp>
#include <cNetworkServer.hpp>

bool RecieveData(cNetworkConnection* pConnection, byte* & buffer, int & iRecievedContent)
{
    int iSize = 0;
    const long recievedSize = pConnection->ReceiveBytes(((byte *) &iSize), 4);
    if (recievedSize != 4) std::cout << "didn't recieve header." << std::endl;
    buffer = new byte[iSize];
    while (iRecievedContent != iSize)
    {
        iRecievedContent += pConnection->ReceiveBytes(buffer + iRecievedContent, iSize - iRecievedContent);
        if (iRecievedContent == -1) iRecievedContent += 1;
    }
    return true;
}

bool SendData(cNetworkConnection* pConnection, byte* buffer, int iSize)
{
    if(!pConnection->SendBytes((byte*)&iSize, 4)) return false;
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

    std::string_view sBuffer((char*)buffer, 36);
    glm::vec3* oPos = (glm::vec3*)&buffer[36];
    glm::vec3* oRot = (glm::vec3*) & buffer[36 + sizeof(glm::vec3)];
    for(uint i = 0; i < aConnections.size(); i++)
        if (aConnections[i] != pConnection)
        {
            if (!SendData(aConnections[i], buffer, iRecievedContent))
            {
                std::cout << "failed sending to " << aConnections[i]->GetConnectionString() << ", error: " << aConnections[i]->piFailures << std::endl;;
                if (aConnections[i]->piFailures++ >= 5)
                {
                    std::cout << "terminated connection " << aConnections[i]->GetConnectionString() << std::endl;;
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
