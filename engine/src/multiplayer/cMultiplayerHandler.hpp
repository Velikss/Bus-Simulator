#pragma once
#include <pch.hpp>
#include <multiplayer/cNetworkClient.hpp>
#include "StdUuid.hpp"
#include <vulkan/entities/cBus.hpp>

bool RecieveData(cNetworkConnection* pConnection, byte*& buffer, int& iRecievedContent)
{
    int iSize = 0;
    const long recievedSize = pConnection->ReceiveBytes(((byte*)&iSize), 4);
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
    if (!pConnection->SendBytes((byte*)&iSize, 4)) return false;
    if (!pConnection->SendBytes(buffer, iSize)) return false;
    return true;
}

bool SendData(cNetworkClient* pConnection, byte* buffer, int iSize)
{
    if (!pConnection->SendBytes((byte*)&iSize, 4)) return false;
    if (!pConnection->SendBytes(buffer, iSize)) return false;
    return true;
}

class cMultiplayerHandler : protected cNetworkClient
{
protected:
    cScene* ppScene = nullptr;
    string psGameServerUuid;
    byte* pBuffer = new byte[36 + (sizeof(glm::vec3) * 2)];
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
        return Connect();
    }

    void PushData()
    {
        if(!ppScene) return;

        static const int pos_pos = 36;
        static const int rot_pos = pos_pos + sizeof(glm::vec3);
        static const int msg_size = 36 + (sizeof(glm::vec3) * 2);

        cBus* oBus = (cBus*)ppScene->GetObjects()["bus"];
        glm::vec3* oPos = oBus->getPosition();
        glm::vec3* oRot = oBus->getRotation();
        memcpy(pBuffer + pos_pos, oPos, sizeof(glm::vec3));
        memcpy(pBuffer + rot_pos, oRot, sizeof(glm::vec3));
        SendData(this, pBuffer, msg_size);
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

bool cMultiplayerHandler::OnRecieve(cNetworkConnection* pConnection)
{
    byte* buffer = nullptr;
    int iRecievedContent = 0;
    if (!RecieveData(pConnection, buffer, iRecievedContent))
        return false;

    std::string sId((char*)buffer, 36);
    auto& aObjects = ppScene->GetObjects();

    if (aObjects.find(sId) == aObjects.end())
    {
        auto& aMeshes = ppScene->GetMeshes();
        aObjects[sId] = new cBus(aMeshes["bus"]);
        aObjects[sId]->setScale(glm::vec3(0.8, 0.8, 0.8));
    }

    glm::vec3* oPos = (glm::vec3*) & buffer[36];
    glm::vec3* oRot = (glm::vec3*) & buffer[36 + sizeof(glm::vec3)];

    std::cout << sId << ", x: " << oPos->x << ", y: " << oPos->y << ", z: " << oPos->z <<
        "rot-x: " << oRot->x << ", rot-y: " << oRot->y << ", rot-z: " << oRot->z << std::endl;

    auto& oObject = aObjects[sId];
    oObject->setPosition(*oPos);
    oObject->setRotation(*oRot);
    return true;
}

void cMultiplayerHandler::OnDisconnect(cNetworkConnection *pConnection)
{
    std::cout << "disconnected client." << std::endl;
}
