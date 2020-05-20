#pragma once
#include <pch.hpp>
#include <server/src/ODBC/cODBCInstance.hpp>
#include <server/src/SSO/SsoService.hpp>

class cGameServer : public cSsoService
{
    std::map<string, bool> aRequiredTables {
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
                if (aRequiredTables.find(sTableName) != aRequiredTables.end())
                    aRequiredTables[sTableName] = true;
            }

        for(auto& [sKey, bExists] : aRequiredTables)
        {
            if(!bExists)
                if(!poDB->ExecFile("./SQL/CreateGame" + sKey + ".sql"))
                    return false;
                else
                    aRequiredTables[sKey] = true;
        }
        return true;
    }

    bool DestroyDB()
    {
        for(auto& [sKey, bExists] : aRequiredTables)
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
    SSO_STATUS iStatus = HandleSession(pConnection);
    if (iStatus == C_SSO_DISCONNECT) return false;
    if (iStatus == C_SSO_NOHANDLE) return true;
    if (iStatus == C_SSO_LOGIN_OK)
    {
        WhiteListConnection(pConnection);
        return true;
    }

    if (iStatus == C_SSO_OK)
    {
        std::cout << "session data found." << std::endl;
        std::cout << "WhiteListed: " << IsWhiteListed(pConnection) << std::endl;
    }
    return false;
}

void cGameServer::OnDisconnect(cNetworkConnection *pConnection)
{

}
