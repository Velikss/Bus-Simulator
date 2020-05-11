#pragma once
#include <pch.hpp>
#include <server/src/ODBC/cODBCInstance.hpp>
#include <server/src/cNetworkServer.hpp>

class cSSOServer : protected cNetworkServer
{
    std::map<string, bool> aRequiredTables {
            {"User", false}
    };
    std::shared_ptr<cODBCInstance> poDB;
public:
    cSSOServer(cNetworkConnection::tNetworkInitializationSettings* tSettings) : cNetworkServer(tSettings)
    {
        std::function<bool(cNetworkConnection*)> _OnConnect = std::bind(&cSSOServer::OnConnect, this, std::placeholders::_1);
        std::function<bool(cNetworkConnection*)> _OnRecieve = std::bind(&cSSOServer::OnRecieve, this, std::placeholders::_1);
        std::function<void(cNetworkConnection*)> _OnDisconnect = std::bind(&cSSOServer::OnDisconnect, this, std::placeholders::_1);
        SetOnConnectEvent(_OnConnect);
        SetOnRecieveEvent(_OnRecieve);
        SetOnDisconnectEvent(_OnDisconnect);
    }

    bool Init(const string& sODBCConnectionString)
    {
        poDB = std::make_shared<cODBCInstance>();
        std::vector<SQLROW> aTables;
        if(!poDB->Connect(sODBCConnectionString))
            return false;
        if(!poDB->Fetch("SHOW TABLES", &aTables))
            return false;
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
                if(!poDB->ExecFile("./SQL/CreateSSO" + sKey + ".sql"))
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

bool cSSOServer::OnConnect(cNetworkConnection* pConnection)
{
    return true;
}

bool cSSOServer::OnRecieve(cNetworkConnection *pConnection)
{
    return true;
}

void cSSOServer::OnDisconnect(cNetworkConnection *pConnection)
{

}
