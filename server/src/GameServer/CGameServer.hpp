#pragma once
#include <pch.hpp>
#include <server/src/ODBC/cODBCInstance.hpp>
#include <server/src/cNetworkServer.hpp>

class cGameServer : protected cNetworkServer
{
    std::map<string, bool> aRequiredTables {
            //{"User", false}
    };
    std::shared_ptr<cODBCInstance> poDB;
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

    bool Init(string sODBCConnectionString)
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
    byte* aBytes = new byte[8192];
    size_t size = pConnection->ReceiveBytes(aBytes, 8192);
    std::string_view sBytes((const char*)aBytes, size);
    cHttp::cRequest oRequest;
    cHttp::cRequest::DeserializeMeta(sBytes, oRequest);

    int iContentLength = std::atoi(oRequest.GetHeader("content-length").c_str());
    int iRequestLength = iContentLength + oRequest.GetMetaLength();
    if (iRequestLength >= 8192)
    {
        byte* aNewBuffer = new byte[iRequestLength];
        memcpy(aNewBuffer, aBytes, sBytes.size());
        delete[] aBytes;
        aBytes = aNewBuffer;
        sBytes = std::string_view((const char*)aBytes, size);
    }

    cHttp::cRequest::DeserializeContent(sBytes, oRequest);

    size_t lMissingContent = 0;
    while ((lMissingContent = oRequest.GetMissingContent()) != 0)
    {
        size = pConnection->ReceiveBytes(aBytes, lMissingContent);
        cHttp::cRequest::DeserializeContent(sBytes, oRequest);
    }

    cUri oUri = cUri::ParseFromRequest(oRequest.GetResource());

    return true;
}

void cGameServer::OnDisconnect(cNetworkConnection *pConnection)
{

}
