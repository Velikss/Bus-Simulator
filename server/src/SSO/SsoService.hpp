#pragma once
#include <pch.hpp>
#include <server/src/NetworkServer.hpp>
#include <server/src/NetworkClient.hpp>

using namespace cHttp;

class cSSOService : public cNetworkServer
{
protected:
    string psSSOUuid;
    std::map<string, cNetworkConnection*> paSessions;
    std::set<cNetworkConnection*> paIgnoredConnections;

    std::shared_ptr<cNetworkClient> poSSOClient = nullptr;
    std::shared_ptr<cNetworkConnection::tNetworkInitializationSettings> pttSSOClientSettings = nullptr;
public:
    cSSOService(cNetworkConnection::tNetworkInitializationSettings* ptNetworkSettings) : cNetworkServer(ptNetworkSettings)
    {
    }

    bool ConnectToSSOServer(const string& sUuid, const string& sIp, const unsigned short& usPort);
    SSO_STATUS HandleSession(cNetworkConnection* pConnection, cRequest & oRequest);
    void WhiteListConnection(cNetworkConnection* pConnection)
    {
        if (paIgnoredConnections.find(pConnection) != paIgnoredConnections.end())
            return;

        paIgnoredConnections.insert(pConnection);
    }
    void UnWhiteListConnection(cNetworkConnection* pConnection)
    {
        if (paIgnoredConnections.find(pConnection) == paIgnoredConnections.end())
            return;
        paIgnoredConnections.erase(pConnection);
    }
    bool IsWhiteListed(cNetworkConnection* pConnection)
    {
        return paIgnoredConnections.find(pConnection) != paIgnoredConnections.end();
    }
private:
    bool SessionExists(const string& sSessionKey, cNetworkConnection* pConnection)
    {
        if (paIgnoredConnections.find(pConnection) != paIgnoredConnections.end())
            return true;

        // Normal lookup sessions.
        auto iIndex = paSessions.find(sSessionKey);
        if (iIndex == paSessions.end()) return false;
        return iIndex->second == pConnection;
    }

    bool RequestSession(cNetworkConnection* pConnection, cResponse & oAwnser, const string& sProvidedSessionKeyOpt = "")
    {
        if(!poSSOClient->IsConnected()) return false;

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
        poSSOClient->SendBytes((byte*)sRequest.c_str(), (int) sRequest.size());

        return cHttp::RecieveResponse(poSSOClient.get(), oAwnser, 250);
    }

    void _OnConnect(cNetworkConnection* pConnection);
    bool _OnRecieve(cNetworkConnection* pConnection);
    void _OnDisconnect(cNetworkConnection* pConnection);
};

void cSSOService::_OnConnect(cNetworkConnection *pConnection)
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
    poSSOClient->SendBytes((byte*)sRequest.c_str(), (int) sRequest.size());

    cResponse oResponse;
    RecieveResponse(pConnection, oResponse);

    if (oResponse.GetResponseCode() != 200) throw std::runtime_error("inaccessible sso server, is the Service-ID correct?");
}

bool cSSOService::_OnRecieve(cNetworkConnection *pConnection)
{
    return true;
}

void cSSOService::_OnDisconnect(cNetworkConnection *pConnection)
{

}

SSO_STATUS cSSOService::HandleSession(cNetworkConnection *pConnection, cRequest & oRequest)
{
    using namespace cHttp;
    cResponse oClientAwnser;
    if (!cHttp::RecieveRequest(pConnection, oRequest)) return C_SSO_ERR;
    cUri oUri = cUri::ParseFromRequest(oRequest.GetResource());
    const string sSessionKey = oRequest.GetHeader("session-key");

    if (oUri.pasPath.size() == 0) // If the application doesn't know what it's doing just terminate the connection after sending a blank 404
    {
        oClientAwnser.SetResponseCode(404);
        string sBuffer = oClientAwnser.Serialize();
        pConnection->SendBytes((byte*)sBuffer.c_str(), (int) sBuffer.size());
        return C_SSO_DISCONNECT;
    }

    if(oUri.pasPath[0] == "sso")
    {
        oRequest.SetHeader("client-ip", pConnection->GetConnectionString());
        string sBuffer = oRequest.Serialize();
        poSSOClient->SendBytes((byte*)sBuffer.c_str(), (int) sBuffer.size());
        cResponse oResponse;
        cHttp::RecieveResponse(poSSOClient.get(), oResponse, -1);

        if(oResponse.GetResponseCode() == 200 && oUri.pasPath.size() > 2)
        {
            if (oUri.pasPath[1] == "session" && oUri.pasPath[2] == "request")
            {
                paSessions.insert({oResponse.GetHeader("session-key"), pConnection});
                string sResponseBuffer = oResponse.Serialize();
                pConnection->SendBytes((byte*)sResponseBuffer.c_str(), (int) sResponseBuffer.size());
                return C_SSO_LOGIN_OK;
            }
            if (oUri.pasPath[1] == "session" && oUri.pasPath[2] == "destroy")
            {
                paSessions.erase(oResponse.GetHeader("session-key"));
                string sResponseBuffer = oResponse.Serialize();
                pConnection->SendBytes((byte*)sResponseBuffer.c_str(), (int) sResponseBuffer.size());
                return C_SSO_LOGIN_OK;
            }
        }
        string sResponseBuffer = oResponse.Serialize();
        pConnection->SendBytes((byte*)sResponseBuffer.c_str(), (int) sResponseBuffer.size());

        return C_SSO_NOHANDLE;
    }

    if (!SessionExists(sSessionKey, pConnection))
    {
        cResponse oAwnser;
        bool bSuccess = RequestSession(pConnection, oAwnser, sSessionKey);
        if (bSuccess && oAwnser.GetResponseCode() == 200 &&
            oAwnser.GetHeader("client-ip") == pConnection->GetIP() &&
            oAwnser.GetHeader("session-key") == sSessionKey)
        {
            paSessions.insert({oAwnser.GetHeader("session-key"), pConnection});
            return C_SSO_OK;
        }
            // If a login is required.
        else
        {
            string sBuffer = oAwnser.Serialize();
            pConnection->SendBytes((byte*)sBuffer.c_str(), (int) sBuffer.size());
        }
        return C_SSO_NOHANDLE;
    }

    return C_SSO_OK;
}

bool cSSOService::ConnectToSSOServer(const string &sUuid, const string &sIp, const unsigned short &usPort)
{
    psSSOUuid = sUuid;

    pttSSOClientSettings = std::make_shared<cNetworkConnection::tNetworkInitializationSettings>();
    pttSSOClientSettings->sAddress = sIp;
    pttSSOClientSettings->usPort = usPort;
    pttSSOClientSettings->eIPVersion = cNetworkConnection::cIPVersion::eV4;
    pttSSOClientSettings->eConnectionType = cNetworkConnection::cConnectionType::eTCP;
    pttSSOClientSettings->eMode = cNetworkConnection::cMode::eNonBlocking;

    poSSOClient = std::make_shared<cNetworkClient>(pttSSOClientSettings.get());

    std::function<void(cNetworkConnection *)> _OnConnect = std::bind(&cSSOService::_OnConnect, this,
                                                                     std::placeholders::_1);
    std::function<bool(cNetworkConnection *)> _OnRecieve = std::bind(&cSSOService::_OnRecieve, this,
                                                                     std::placeholders::_1);
    std::function<void(cNetworkConnection *)> _OnDisconnect = std::bind(&cSSOService::_OnDisconnect, this,
                                                                        std::placeholders::_1);

    poSSOClient->SetOnConnectEvent(_OnConnect);
    poSSOClient->SetOnRecieveEvent(_OnRecieve);
    poSSOClient->SetOnDisconnectEvent(_OnDisconnect);

    if (poSSOClient->Connect())
        return true;
    else
    {
        poSSOClient->Disconnect();
        return false;
    }
}
