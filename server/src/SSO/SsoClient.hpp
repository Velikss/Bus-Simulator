#pragma once
#include <pch.hpp>
#include <server/src/NetworkClient.hpp>
#include <server/src/Http/HTTP.hpp>

using namespace cHttp;

class cSSOClient : public cNetworkClient
{
    string psSessionKey = "";
public:
    cSSOClient(tNetworkInitializationSettings* ptSettings) : cNetworkClient(ptSettings)
    {
    }

    bool Login(const string& sLoginName, const string& sPassword, size_t uiTimeOut = 300);
    bool Logout();
    bool SendRequest(cRequest &oRequest, cResponse& oResponse, int uiTimeOut = 300);
};

bool cSSOClient::Login(const string &sLoginName, const string &sPassword, size_t uiTimeOut)
{
    cRequest oRequest;
    cResponse oResponse;

    std::vector<cHeader> aHeaders;

    aHeaders.push_back({"Host", GetIP()});
    aHeaders.push_back({"Connection", "keep-alive"});
    aHeaders.push_back({"Loginname", sLoginName});
    aHeaders.push_back({"Password", sPassword});

    oRequest.SetMethod(cMethod::ePOST);
    oRequest.SetResource("/sso/session/request");
    oRequest.SetHeaders(aHeaders);

    if(!SendRequest(oRequest, oResponse)) return false;
    psSessionKey = oResponse.GetHeader("session-key");
    return oResponse.GetResponseCode() == 200;
}

bool cSSOClient::Logout()
{
    cRequest oRequest;
    cResponse oResponse;

    std::vector<cHeader> aHeaders;

    aHeaders.push_back({"Host", GetIP()});
    aHeaders.push_back({"Connection", "keep-alive"});

    oRequest.SetMethod(cMethod::ePOST);
    oRequest.SetResource("/sso/session/destroy");
    oRequest.SetHeaders(aHeaders);

    SendRequest(oRequest, oResponse, -1);
    psSessionKey.clear();
    return oResponse.GetResponseCode() == 200;
}

bool cSSOClient::SendRequest(cRequest &oRequest, cResponse& oResponse, int uiTimeOut)
{
    oRequest.SetHeader("session-key" , psSessionKey);
    string sRequest = oRequest.Serialize();
    SendBytes((const byte*)sRequest.c_str(), (int) sRequest.size());
    return RecieveResponse(this, oResponse, uiTimeOut);
}
