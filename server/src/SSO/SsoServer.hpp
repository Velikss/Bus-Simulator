#pragma once
#include <pch.hpp>
#include <server/src/ODBC/cODBCInstance.hpp>
#include <server/src/cNetworkServer.hpp>

using namespace cHttp;

class cSSOServer : protected cNetworkServer
{
    std::vector<string> aRequiredTables {
            "User",
            "Session"
    };
    std::shared_ptr<cODBCInstance> poDB;
    std::map<string, cNetworkConnection*> paServices;
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

    bool InitDB(const string& sODBCConnectionString)
    {
        poDB = std::make_shared<cODBCInstance>();
        if(!poDB->Connect(sODBCConnectionString))
            return false;

        for(auto& sKey : aRequiredTables)
        {
            std::cout << "creating table: " << sKey << std::endl;
            if (!poDB->ExecFile("./SQL/CreateSSO" + sKey + ".sql"))
                return false;
        }
        return true;
    }

    bool DestroyDB()
    {
        for (auto iter = aRequiredTables.rbegin(); iter != aRequiredTables.rend(); ++iter) {
            std::cout << "dropping: " << *iter << std::endl;
            if (!poDB->Exec("DROP TABLE IF EXISTS " + *iter + ";"))
                return false;
        }
        return true;
    }

    void RegisterUuid(const string& sUuid)
    {
        paServices.insert({sUuid, nullptr});
    }

    bool Listen()
    {
        return cNetworkServer::Listen();
    }

    void Stop()
    {
        cNetworkServer::Stop();
    }

protected:
    bool OnConnect(cNetworkConnection* pConnection);
    bool OnRecieve(cNetworkConnection* pConnection);
    void OnDisconnect(cNetworkConnection* pConnection);

    bool HandleSessionRequest(cNetworkConnection* pConnection, cUri & oUri, cRequest& oRequest);
};

bool cSSOServer::OnConnect(cNetworkConnection* pConnection)
{
    pConnection->LockRecieve();

    cRequest oRequest;
    if(!cHttp::RecieveRequest(pConnection, oRequest)) return false;
    cUri oUri = cUri::ParseFromRequest(oRequest.GetResource());
    if(oUri.pasPath.size() != 1)
        if(oUri.pasPath[0] != "sso") return false;

    cResponse oResponse;
    string sServiceId = oRequest.GetHeader("service-id");
    if (paServices.find(oRequest.GetHeader("service-id")) != paServices.end())
    {
        if(paServices[sServiceId] != nullptr)
        {
            oResponse.SetResponseCode(403);
            std::cout << "service with Service-ID: '" << sServiceId << "', is still active? blocking..." << std::endl;
        }
        else
        {
            oResponse.SetResponseCode(200);
            paServices[sServiceId] = pConnection;
            std::cout << "service with Service-ID: '" << sServiceId << "', has connected." << std::endl;
        }
    }
    else
    {
        oResponse.SetResponseCode(403);
        std::cout << "service with Service-ID: '" << sServiceId << "', hasn't been registered yet." << std::endl;
    }
    string sResponse = oResponse.Serialize();
    pConnection->SendBytes((byte*)sResponse.c_str(), sResponse.size());

    pConnection->UnLockRecieve();
    return oResponse.GetResponseCode() == 200;
}

bool cSSOServer::OnRecieve(cNetworkConnection *pConnection)
{
    cRequest oRequest;
    if(!cHttp::RecieveRequest(pConnection, oRequest)) return false;
    cUri oUri = cUri::ParseFromRequest(oRequest.GetResource());
    if(oUri.pasPath.size() != 1)
        if(oUri.pasPath[0] != "sso") return false;

    if (oUri.pasPath.size() <= 1) return false;

    if(oUri.pasPath[1] == "session") return HandleSessionRequest(pConnection, oUri, oRequest);

    return false;
}

void cSSOServer::OnDisconnect(cNetworkConnection *pConnection)
{

}

bool cSSOServer::HandleSessionRequest(cNetworkConnection *pConnection, cUri & oUri, cRequest &oRequest)
{
    if(oUri.pasPath.size() < 2) return false;

    cResponse oResponse;
    std::vector<cHeader> aHeaders;
    aHeaders.push_back({"Server", "Orange-SSO"});
    aHeaders.push_back({"Connection", "keep-alive"});

    if(oUri.pasPath[2] == "require") // If a session is requested.
    {
        std::cout << "session requested." << std::endl;
        string sSessionKey = oRequest.GetHeader("session-key");
        string sIp = oRequest.GetHeader("client-ip");
        std::vector<SQLROW> aSessions;
        if (sSessionKey.size() > 0) poDB->Fetch("SELECT * FROM Session WHERE Session.Key = '" + oRequest.GetHeader("session-key") + "' AND Session.Ip = '" + sIp + "';", &aSessions);
        else std::cout << "no key or ip provided: session: " << sSessionKey << ", ip: " << sIp << std::endl;

        if (aSessions.size() == 0)
            oResponse.SetResponseCode(403);
        else // on success with 1 session.
        {
            oResponse.SetResponseCode(200);
            string sDBIp, sDBSessionKey;
            aSessions[0]["Ip"]->GetValueStr(sIp);
            aSessions[0]["Key"]->GetValueStr(sDBSessionKey);
            aHeaders.push_back({"Client-ip", sDBIp});
            aHeaders.push_back({"Session-Key", sDBSessionKey});
        }
    }

    oResponse.SetHeaders(aHeaders);

    string sResponse = oResponse.Serialize();
    pConnection->SendBytes((byte*)sResponse.c_str(), sResponse.size());
    return true;

    return false;
}
