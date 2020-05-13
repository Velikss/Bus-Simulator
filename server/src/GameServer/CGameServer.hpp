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
    using namespace cHttp;
    cRequest oRequest;
    if (!cHttp::RecieveRequest(pConnection, oRequest)) return false;
    cUri oUri = cUri::ParseFromRequest(oRequest.GetResource());
    const string sSessionKey = oRequest.GetHeader("session-key");

    if (!SessionExists(sSessionKey))
    {
        cResponse oAwnser, oClientAwnser;
        bool bSuccess = RequestSession(pConnection, oAwnser, sSessionKey);
        if (bSuccess && oAwnser.GetResponseCode() == 200 &&
            oAwnser.GetHeader("client-ip") == pConnection->GetIP() &&
            oAwnser.GetHeader("session-key") == sSessionKey)
        std::cout << "recieved SSO-awnser: " << bSuccess << ", " << oAwnser.GetResponseCode() << std::endl;
    }

    // the session-data was found.

    return true;
}

void cGameServer::OnDisconnect(cNetworkConnection *pConnection)
{

}
