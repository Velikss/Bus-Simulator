#pragma once
#include <pch.hpp>
#include <server/src/cNetworkServer.hpp>
#include <server/src/cNetworkClient.hpp>

using namespace cHttp;

class cSsoService : public cNetworkServer
{
protected:
    string psSSOUuid;
    std::map<string, cNetworkConnection*> paSessions;

    std::shared_ptr<cNetworkClient> pSSOClient = nullptr;
    std::shared_ptr<cNetworkConnection::tNetworkInitializationSettings> ptSSOClientSettings = nullptr;
public:
    cSsoService(cNetworkConnection::tNetworkInitializationSettings* ptNetworkSettings) : cNetworkServer(ptNetworkSettings)
    {
    }

    bool SessionExists(string sSessionKey)
    {
        return (sSessionKey.size() > 0 && paSessions.find(sSessionKey) != paSessions.end());
    }

    bool RequestSession(cNetworkConnection* pConnection, cResponse & oAwnser, const string& sProvidedSessionKeyOpt = "")
    {
        if(!pSSOClient->IsConnected()) return false;

        // If no session-key was provided or if the session was not found on the server, make a call to the sso server.
        cRequest oRequest;
        std::vector<cHeader> aHeaders;

        aHeaders.push_back({"Host", "127.0.0.1"});
        aHeaders.push_back({"Connection", "keep-alive"});
        aHeaders.push_back({"Client-ip", pConnection->GetIP()});
        if (sProvidedSessionKeyOpt.size() > 0) aHeaders.push_back({"session-key", sProvidedSessionKeyOpt});

        oRequest.SetMethod(cMethod::ePOST);
        oRequest.SetResource("/sso/session/require");
        oRequest.SetHeaders(aHeaders);

        string sRequest = oRequest.Serialize();
        pSSOClient->SendBytes((byte*)sRequest.c_str(), sRequest.size());

        return cHttp::RecieveResponse(pSSOClient.get(), oAwnser, 250);
    }

    bool ConnectToSSOServer(const string& sUuid, const string& sIp, const unsigned short& usPort)
    {
        psSSOUuid = sUuid;

        ptSSOClientSettings = std::make_shared<cNetworkConnection::tNetworkInitializationSettings>();
        ptSSOClientSettings->sAddress = sIp;
        ptSSOClientSettings->usPort = usPort;
        ptSSOClientSettings->eIPVersion = cNetworkConnection::cIPVersion::eV4;
        ptSSOClientSettings->eConnectionType = cNetworkConnection::cConnectionType::eTCP;
        ptSSOClientSettings->eMode = cNetworkConnection::cMode::eNonBlocking;

        pSSOClient = std::make_shared<cNetworkClient>(ptSSOClientSettings.get());

        std::function<void(cNetworkConnection*)> _OnConnect = std::bind(&cSsoService::_OnConnect, this, std::placeholders::_1);
        std::function<bool(cNetworkConnection*)> _OnRecieve = std::bind(&cSsoService::_OnRecieve, this, std::placeholders::_1);
        std::function<void(cNetworkConnection*)> _OnDisconnect = std::bind(&cSsoService::_OnDisconnect, this, std::placeholders::_1);

        pSSOClient->SetOnConnectEvent(_OnConnect);
        pSSOClient->SetOnRecieveEvent(_OnRecieve);
        pSSOClient->SetOnDisconnectEvent(_OnDisconnect);

        if(pSSOClient->Connect())
            return true;
        else
        {
            pSSOClient->Disconnect();
            return false;
        }
    }
private:
    void _OnConnect(cNetworkConnection* pConnection);
    bool _OnRecieve(cNetworkConnection* pConnection);
    void _OnDisconnect(cNetworkConnection* pConnection);
};

void cSsoService::_OnConnect(cNetworkConnection *pConnection)
{
    // If no session-key was provided or if the session was not found on the server, make a call to the sso server.
    cRequest oRequest;
    std::vector<cHeader> aHeaders;

    aHeaders.push_back({"Host", "127.0.0.1"});
    aHeaders.push_back({"Connection", "keep-alive"});
    aHeaders.push_back({"Service-ID", psSSOUuid});

    oRequest.SetMethod(cMethod::ePOST);
    oRequest.SetResource("/sso");
    oRequest.SetHeaders(aHeaders);

    string sRequest = oRequest.Serialize();
    pSSOClient->SendBytes((byte*)sRequest.c_str(), sRequest.size());

    cResponse oResponse;
    RecieveResponse(pConnection, oResponse);

    if (oResponse.GetResponseCode() != 200) throw std::runtime_error("inaccessible sso server, is the Service-ID correct?");
}

bool cSsoService::_OnRecieve(cNetworkConnection *pConnection)
{
    return true;
}

void cSsoService::_OnDisconnect(cNetworkConnection *pConnection)
{

}
