#pragma once

#include <pch.hpp>
#include <entities/cBus.hpp>
#include <StdUuid.hpp>
#include <GameServer/GameConnectionHelper.hpp>
#include <SSO/SsoClient.hpp>

struct tFixedVec3
{
    float x;
    float y;
    float z;
};

static tFixedVec3 ToFixedVec(glm::vec3 pVec)
{
    return {
            pVec.x,
            pVec.y,
            pVec.z
    };
}

static glm::vec3 ToGLMVec(tFixedVec3* pVec)
{
    return {
            pVec->x,
            pVec->y,
            pVec->z
    };
}

class cMultiplayerHandler : public cSSOClient
{
protected:
    cScene* ppScene = nullptr;
    string psGameServerUuid;
    bool pbPushData = false;
    byte* pBuffer = new byte[36 + (sizeof(glm::vec3) * 2)];
    std::map<std::string, std::string> pmsBusIds;
    std::stack<uint> paAvailableBusses;
public:
    cMultiplayerHandler(cNetworkConnection::tNetworkInitializationSettings* pSettings) : cSSOClient(pSettings)
    {
        psGameServerUuid = uuids::to_string(uuids::uuid_system_generator{}());
        memcpy(pBuffer, psGameServerUuid.c_str(), 36);

        std::function<void(cNetworkConnection*)> _OnConnect = std::bind(&cMultiplayerHandler::OnConnect, this,
                                                                        std::placeholders::_1);
        std::function<bool(cNetworkConnection*)> _OnRecieve = std::bind(&cMultiplayerHandler::OnRecieve, this,
                                                                        std::placeholders::_1);
        std::function<void(cNetworkConnection*)> _OnDisconnect = std::bind(&cMultiplayerHandler::OnDisconnect, this,
                                                                           std::placeholders::_1);
        SetOnConnectEvent(_OnConnect);
        SetOnRecieveEvent(_OnRecieve);
        SetOnDisconnectEvent(_OnDisconnect);

        for (uint i = 0; i < 10; i++) paAvailableBusses.push(i);
    }

    ~cMultiplayerHandler() override
    {
        Stop();
        delete[] pBuffer;
    }

    bool Start()
    {
        return Connect();
    }

    void AssignScene(cScene* pScene)
    {
        ppScene = pScene;
    }
    void ClearScene()
    {
        ppScene = nullptr;
    }

    void PushData()
    {
        if (!ppScene || !pbConnectionAcitve || !pbPushData) return;

        static const int pos_pos = 36;
        static const int rot_pos = pos_pos + sizeof(tFixedVec3);
        static const int msg_size = 36 + (sizeof(tFixedVec3) * 2);

        cBus* oBus = (cBus*) ppScene->GetObjects()["bus"];
        glm::vec3 oPos = oBus->GetPosition();
        glm::vec3 oRot = oBus->GetRotation();
        tFixedVec3 fixedPos = ToFixedVec(oPos);
        tFixedVec3 fixedRot = ToFixedVec(oRot);
        memcpy(pBuffer + pos_pos, &fixedPos, sizeof(tFixedVec3));
        memcpy(pBuffer + rot_pos, &fixedRot, sizeof(tFixedVec3));
        nGameConnectionHelper::SendData(this, pBuffer, msg_size);
    }

    void Stop()
    {
        cNetworkClient::Disconnect();
    }

    bool RegisterUser(const string& sUserName, const string& sPassword);
    void StartMultiplayerSession();
protected:
    void OnConnect(cNetworkConnection* pConnection);
    bool OnRecieve(cNetworkConnection* pConnection);
    void OnDisconnect(cNetworkConnection* pConnection);
};

void cMultiplayerHandler::OnConnect(cNetworkConnection* pConnection)
{
    ENGINE_LOG("Connected client");
}

bool cMultiplayerHandler::OnRecieve(cNetworkConnection* pConnection)
{
    if(!pbConnectionAcitve) return true;

    using namespace nGameConnectionHelper;

    byte* buffer = nullptr;
    int iRecievedContent = 0;
    if (!RecieveData(pConnection, buffer, iRecievedContent))
        return false;

    if (!ppScene) return true;
    auto pScene = ppScene;

    std::string sId((char*) buffer, 36);

    auto& aObjects = pScene->GetObjects();

    if (pmsBusIds.count(sId) == 0)
    {
        pmsBusIds[sId] = "multiplayer_bus_" + std::to_string(paAvailableBusses.top());
        paAvailableBusses.pop();
        aObjects[pmsBusIds[sId]]->SetScale(glm::vec3(0.8, 0.8, 0.8));
    }

    tFixedVec3* pFixedPos = (tFixedVec3*) &buffer[36];
    tFixedVec3* pFixedRot = (tFixedVec3*) &buffer[36 + sizeof(tFixedVec3)];

    std::cout << sId << ", x: " << pFixedPos->x << ", y: " << pFixedPos->y << ", z: " << pFixedPos->z <<
              "rot-x: " << pFixedRot->x << ", rot-y: " << pFixedRot->y << ", rot-z: " << pFixedRot->z << std::endl;

    glm::vec3 oPos = ToGLMVec(pFixedPos);
    glm::vec3 oRot = ToGLMVec(pFixedRot);

    auto& oObject = aObjects[pmsBusIds[sId]];
    oObject->SetPosition(oPos);
    oObject->SetRotation(oRot);
    dynamic_cast<cBus*>(oObject)->piPingTimeout = 0;

    for (auto& object : pmsBusIds)
    {
        auto bus = dynamic_cast<cBus*>(aObjects[object.second]);
        bus->piPingTimeout++;
        if (bus->piPingTimeout > 50)
        {
            paAvailableBusses.push(bus->piBusId);
            bus->SetScale(glm::vec3(0));
        }
    }

    return true;
}

void cMultiplayerHandler::OnDisconnect(cNetworkConnection* pConnection)
{
    ENGINE_LOG("Disconnected client");
}

bool cMultiplayerHandler::RegisterUser(const string& sUserName, const string& sPassword)
{
    cRequest oRequest;
    cResponse oResponse;
    std::vector<cHeader> aHeaders;
    aHeaders.push_back({"Connection", "keep-alive"});

    oRequest.SetMethod(cMethod::ePOST);
    oRequest.SetResource("/sso/account/create");
    oRequest.SetHeader("loginname", sUserName);
    oRequest.SetHeader("password", sPassword);

    SendRequest(oRequest, oResponse, -1);
    return oResponse.GetResponseCode() == 200;
}

void cMultiplayerHandler::StartMultiplayerSession()
{
    cRequest oRequest;
    oRequest.SetMethod(cMethod::ePOST);
    oRequest.SetResource("/game/start");
    oRequest.SetHeader("Connection", "keep-alive");
    oRequest.SetHeader("session-key" , psSessionKey);
    string sRequest = oRequest.Serialize();
    SendBytes((byte*)sRequest.c_str(), (int)sRequest.size());
    pbPushData = true;
}
