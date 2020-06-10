#pragma once
#include <pch.hpp>
#include <NetworkClient.hpp>
#include <Http/HTTP.hpp>

using namespace cHttp;

class cSSOClient : public cNetworkClient
{
protected:
    bool pbConnectionAcitve = false;
    string psSessionKey = "";
public:
    cSSOClient(tNetworkInitializationSettings* ptSettings) : cNetworkClient(ptSettings)
    {
    }
    virtual void Disconnect();
    virtual bool Login(const string& sLoginName, const string& sPassword, size_t uiTimeOut = 300);
    virtual bool Logout();
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

    if(!SendRequest(oRequest, oResponse, -1)) return false;
    psSessionKey = oResponse.GetHeader("session-key");
    if(oResponse.GetResponseCode() == 200)
        pbConnectionAcitve = true;
    return pbConnectionAcitve;
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
    pbConnectionAcitve = false;
    return oResponse.GetResponseCode() == 200;
}

bool cSSOClient::SendRequest(cRequest &oRequest, cResponse& oResponse, int uiTimeOut)
{
    oRequest.SetHeader("session-key" , psSessionKey);
    string sRequest = oRequest.Serialize();
    SendBytes((const byte*)sRequest.c_str(), (int) sRequest.size());
    return RecieveResponse(this, oResponse, uiTimeOut);
}

void cSSOClient::Disconnect()
{
    cNetworkClient::Disconnect();
    this->pbConnectionAcitve = false;
}
