#pragma once
#include <pch.hpp>
#include <multiplayer/cNetworkClient.hpp>
#include "StdUuid.hpp"

void RecieveData(cNetworkConnection* pConnection, byte* & buffer, int & iRecievedContent)
{
    int iSize = 0;
    const long recievedSize = pConnection->ReceiveBytes(((byte *) &iSize), 4);
    if (recievedSize != 4) std::cout << "didn't recieve header." << std::endl;
    buffer = new byte[iSize];
    std::cout << "recieving: " << iSize << std::endl;
    while (iRecievedContent != iSize)
    {
        iRecievedContent += pConnection->ReceiveBytes(buffer + iRecievedContent, iSize - iRecievedContent);
        std::cout << "recieved: " << iRecievedContent << ", out of: " << iSize << std::endl;
    }
}

void SendData(cNetworkConnection* pConnection, byte* buffer, int iSize)
{
    pConnection->SendBytes((byte*)&iSize, 4);
    pConnection->SendBytes(buffer, iSize);
}

void SendData(cNetworkClient* pConnection, byte* buffer, int iSize)
{
    pConnection->SendBytes((byte*)&iSize, 4);
    pConnection->SendBytes(buffer, iSize);
}

class cMultiplayerHandler : protected cNetworkClient
{
protected:
    cScene* ppScene = nullptr;
    std::vector<std::thread> pThreads;
    string psGameServerUuid;
    byte* pBuffer = new byte[36 + sizeof(glm::vec3)];
public:
    cMultiplayerHandler(tNetworkInitializationSettings* pSettings, cScene* pScene) : cNetworkClient(pSettings)
    {
        this->ppScene = pScene;
        psGameServerUuid = uuids::to_string(uuids::uuid_system_generator{}());
        memcpy(pBuffer, psGameServerUuid.c_str(), 36);

        std::function<void(cNetworkConnection*)> _OnConnect = std::bind(&cMultiplayerHandler::OnConnect, this, std::placeholders::_1);
        std::function<bool(cNetworkConnection*)> _OnRecieve = std::bind(&cMultiplayerHandler::OnRecieve, this, std::placeholders::_1);
        std::function<void(cNetworkConnection*)> _OnDisconnect = std::bind(&cMultiplayerHandler::OnDisconnect, this, std::placeholders::_1);
        SetOnConnectEvent(_OnConnect);
        SetOnRecieveEvent(_OnRecieve);
        SetOnDisconnectEvent(_OnDisconnect);
    }

    ~cMultiplayerHandler() override
    {
        Stop();
    }

    bool Start()
    {
        if(!Connect()) return false;
        SendData(this, (byte*)psGameServerUuid.c_str(), psGameServerUuid.size());
        return true;
    }

    void PushData()
    {
        auto& oCamera = ppScene->GetCamera();
        glm::vec3 oPos = oCamera.GetPosition();
        memcpy(pBuffer + 36, &oPos, sizeof(glm::vec3));
        try
        {
            std::cout << "x: " << oPos.x << ", y: " << oPos.y << ", z: " << oPos.z << std::endl;

            SendData(this, pBuffer, 36 + sizeof(glm::vec3));
        }
        catch (std::exception& ex)
        {
            std::cout << ex.what() << std::endl;
        }
    }

    void Stop()
    {
        cNetworkClient::Disconnect();
    }

protected:
    void OnConnect(cNetworkConnection* pConnection);
    bool OnRecieve(cNetworkConnection* pConnection);
    void OnDisconnect(cNetworkConnection* pConnection);
};

void cMultiplayerHandler::OnConnect(cNetworkConnection *pConnection)
{
    std::cout << "connected client." << std::endl;
}

bool cMultiplayerHandler::OnRecieve(cNetworkConnection *pConnection)
{
    std::cout << "client recieving data..." << std::endl;
    byte* buffer = nullptr;
    int iRecievedContent = 0;
    RecieveData(pConnection, buffer, iRecievedContent);
    std::string_view sBuffer((char*)buffer, iRecievedContent);
    std::cout << sBuffer << std::endl;
    return true;
}

void cMultiplayerHandler::OnDisconnect(cNetworkConnection *pConnection)
{
    std::cout << "disconnected client." << std::endl;
}
