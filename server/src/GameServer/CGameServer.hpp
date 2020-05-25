#pragma once
#include <pch.hpp>
#include <server/src/ODBC/ODBCInstance.hpp>
#include <server/src/SSO/SsoService.hpp>

class cGameServer : public cSsoService
{
    std::map<string, bool> pC_aRequiredTables {
            //{"User", false}
    };

    std::shared_ptr<cODBCInstance> poDB;
public:
    cGameServer(cNetworkConnection::tNetworkInitializationSettings* tSettings) : cSsoService(tSettings)
    {
        std::function<bool(cNetworkConnection*)> _OnConnect = std::bind(&cGameServer::OnConnect, this, std::placeholders::_1);
        std::function<bool(cNetworkConnection*)> _OnRecieve = std::bind(&cGameServer::OnRecieve, this, std::placeholders::_1);
        std::function<void(cNetworkConnection*)> _OnDisconnect = std::bind(&cGameServer::OnDisconnect, this, std::placeholders::_1);
        SetOnConnectEvent(_OnConnect);
        SetOnRecieveEvent(_OnRecieve);
        SetOnDisconnectEvent(_OnDisconnect);
    }

    bool InitDB(string sODBCConnectionString)
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

    bool OnConnect(cNetworkConnection* pConnection);
    bool OnRecieve(cNetworkConnection* pConnection);
    void OnDisconnect(cNetworkConnection* pConnection);
};

bool cGameServer::OnConnect(cNetworkConnection* pConnection)
{
    return true;
}

bool cGameServer::OnRecieve(cNetworkConnection *pConnection)
{
    cRequest oRequest;
    SSO_STATUS iStatus = HandleSession(pConnection, oRequest);

    if (iStatus == C_SSO_DISCONNECT)
        return false;
    if (iStatus == C_SSO_NOHANDLE) return true;
    if (iStatus == C_SSO_LOGIN_OK)
    {
        WhiteListConnection(pConnection);
        return true;
    }

    if (iStatus == C_SSO_OK)
    {
        cResponse oResponse;
        oResponse.SetResponseCode(200);
        string sBuffer = oResponse.Serialize();
        pConnection->SendBytes((byte*)sBuffer.c_str(), sBuffer.size());
        return true;
    }
    return false;
}

void cGameServer::OnDisconnect(cNetworkConnection *pConnection)
{

}
