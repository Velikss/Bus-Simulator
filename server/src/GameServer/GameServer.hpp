#pragma once
#include <pch.hpp>
#include <server/src/ODBC/ODBCInstance.hpp>
#include <server/src/SSO/SsoService.hpp>
#include <server/src/GameServer/GameConnectionHelper.hpp>

class cGameServer : public cSSOService
{
    std::map<string, bool> pC_aRequiredTables {
            //{"User", false}
    };

    std::shared_ptr<cODBCInstance> poDB;
    byte paBuffer[255];
public:
    cGameServer(cNetworkConnection::tNetworkInitializationSettings* tSettings) : cSSOService(tSettings)
    {
        std::function<bool(cNetworkConnection*)> _OnConnect = std::bind(&cGameServer::OnConnect, this, std::placeholders::_1);
        std::function<bool(cNetworkConnection*)> _OnRecieve = std::bind(&cGameServer::OnRecieve, this, std::placeholders::_1);
        std::function<void(cNetworkConnection*)> _OnDisconnect = std::bind(&cGameServer::OnDisconnect, this, std::placeholders::_1);
        SetOnConnectEvent(_OnConnect);
        SetOnRecieveEvent(_OnRecieve);
        SetOnDisconnectEvent(_OnDisconnect);
    }

    bool InitDB(const string& sODBCConnectionString)
    {
        poDB = std::make_shared<cODBCInstance>();
        std::vector<SQLROW> aTables;
        if(!poDB->Connect(sODBCConnectionString)) return false;
        if(!poDB->Fetch("SHOW TABLES", &aTables)) return false;
        for(auto& oEntry : aTables)
            for(auto& [sKey, oValue] : oEntry)
            {
                string sTableName;
                oValue->GetValueStr(sTableName);
                if (pC_aRequiredTables.find(sTableName) != pC_aRequiredTables.end())
                    pC_aRequiredTables[sTableName] = true;
            }

        for(auto& [sKey, bExists] : pC_aRequiredTables)
        {
            if(!bExists)
                if(!poDB->ExecFile("./SQL/CreateGame" + sKey + ".sql"))
                    return false;
                else
                    pC_aRequiredTables[sKey] = true;
        }
        return true;
    }

    bool DestroyDB()
    {
        for(auto& [sKey, bExists] : pC_aRequiredTables)
            if(!poDB->Exec("DROP TABLE IF EXISTS " + sKey + ";"))
                return false;
        return true;
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

private:
    bool OnConnect(cNetworkConnection* pConnection);
    bool OnRecieve(cNetworkConnection* pConnection);
    void OnDisconnect(cNetworkConnection* pConnection);
    bool HandleGameConnection(cNetworkConnection* pConnection);
};

bool cGameServer::OnConnect(cNetworkConnection* pConnection)
{
    std::cout << "Client connected: " << pConnection->GetConnectionString() << std::endl;
    return true;
}

bool cGameServer::OnRecieve(cNetworkConnection *pConnection)
{
    cRequest oRequest;
    SSO_STATUS iStatus = HandleSession(pConnection, oRequest);

    if (iStatus == C_SSO_DISCONNECT) return false;
    if (iStatus == C_SSO_NOHANDLE) return true;
    if (iStatus == C_SSO_LOGIN_OK)
    {
        WhiteListConnection(pConnection);
        return true;
    }

    if (iStatus == C_SSO_OK) return HandleGameConnection(pConnection);
    return false;
}

void cGameServer::OnDisconnect(cNetworkConnection *pConnection)
{
    std::cout << "Client disconnected: " << pConnection->GetConnectionString() << std::endl;
}

bool cGameServer::HandleGameConnection(cNetworkConnection* pConnection)
{
    int iRecievedContent = 0;
    if (!nGameConnectionHelper::RecieveData(pConnection, &(paBuffer[0]), iRecievedContent))
        return false;
    std::cout << "recieved data." << std::endl;
    for(uint i = 0; i < paConnections.size(); i++) //-V104
        if (paConnections[i] != pConnection && IsWhiteListed(paConnections[i])) //-V108
        {
            if (!nGameConnectionHelper::SendData(paConnections[i], &(paBuffer[0]), iRecievedContent)) //-V108
            {
                std::cout << "failed sending to " << paConnections[i]->GetConnectionString() << ", error: " << paConnections[i]->piFailures << std::endl;; //-V108
                if (paConnections[i]->piFailures++ >= 5) //-V108
                {
                    std::cout << "terminated connection " << paConnections[i]->GetConnectionString() << std::endl;; //-V108
                    RemoveConnectionAt(i);
                }
            }
        }
    return true;
}
