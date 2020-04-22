#pragma once
#include <cNetworkConnection.hpp>
#include <Http/HTTP.hpp>
#include <vendor/Utf8.hpp>

class cNetworkServer : public cNetworkConnection
{
    std::vector<std::thread> threads;
    std::vector<cNetworkConnection*> aConnections;

    std::function<bool(cNetworkConnection *)> OnConnect = nullptr;
    std::function<bool(cNetworkConnection *)> OnRecieve = nullptr;
public:
	cNetworkServer(cNetworkConnection::tNetworkInitializationSettings* ptNetworkSettings) : cNetworkConnection(ptNetworkSettings)
	{
	}

	void SetUpEvents(const std::function<bool(cNetworkConnection *)> &OnConnect,
	                 const std::function<bool(cNetworkConnection *)> &OnRecieve)
    {
	    this->OnConnect = OnConnect;
	    this->OnRecieve = OnRecieve;
    }

	bool Listen();
private:
    void OnConnectLoop();
    void OnRecieveLoop();
	cNetworkConnection* AcceptConnection(bool bBlockingSocket = false) const;
};

cNetworkConnection *cNetworkServer::AcceptConnection(bool bBlockingSocket) const
{
    sockaddr_in tClientAddr = {};
    socklen_t iClientAddrLength = sizeof(tClientAddr);

    const NET_SOCK oSock = accept(poSock, (struct sockaddr *) &tClientAddr,
                                  &iClientAddrLength);

    if (oSock < 1) return nullptr;

    auto oNewConnection = new cNetworkConnection(oSock, tClientAddr);

    if (pptNetworkSettings->bUseSSL)
    {
        oNewConnection->ppConnectionSSL = SSL_new(ppSSLContext);
        SSL_set_fd(oNewConnection->ppConnectionSSL, oSock);

        int iReturn = SSL_accept(oNewConnection->ppConnectionSSL);
        if (iReturn < 0)
        {
            delete oNewConnection;
            return nullptr;
        }
    }

    if (!bBlockingSocket) cNetworkAbstractions::SetBlocking(oSock, bBlockingSocket);

    return oNewConnection;
}

bool cNetworkServer::Listen()
{
    if ((piLastStatus = bind(poSock, (const sockaddr*)&ptAddress, sizeof(ptAddress))) != 0)
    {
        Close();
        return false;
    }
    if ((piLastStatus = listen(poSock, SOMAXCONN)) != 0)
    {
        Close();
        return false;
    }
    threads.push_back(std::thread(&cNetworkServer::OnConnectLoop, this));
    threads.push_back(std::thread(&cNetworkServer::OnRecieveLoop, this));
    return true;
}

void cNetworkServer::OnConnectLoop()
{
    while (true)
    {
        cNetworkConnection *incoming = nullptr;
        if ((incoming = AcceptConnection(false)) != nullptr)
        {
            bool bPass = true;
            if(OnConnect) bPass = OnConnect(incoming);
            if(bPass)
                aConnections.push_back(incoming);
            else
                delete incoming;
        }
        sleep(1);
    }
}

void cNetworkServer::OnRecieveLoop()
{
    while (true)
    {
        for (uint i = 0; i < aConnections.size(); i++)
        {
            auto status = aConnections[i]->Status();
            if (status == cNetworkAbstractions::cConnectionStatus::eAVAILABLE)
            {
                byte buffer[8192]{0};
                const long size = aConnections[i]->ReceiveBytes((byte *) &buffer[0], 8192);
                if (size <= 0) continue;
                const std::string_view req_str((char*)buffer, size);
                cHttp::cRequest req = cHttp::cRequest::Deserialize((string) req_str);

                Utf8 oUTF8ToHtmlConverter;
                std::ifstream oHtmlStream("./wwwroot/index.html");
                if (!oHtmlStream.is_open())
                {
                    std::cout << "index.html could not be found." << std::endl;
                    break;
                }
                std::string sUTFHtml((std::istreambuf_iterator<char>(oHtmlStream)),
                                     std::istreambuf_iterator<char>());

                auto ex = oUTF8ToHtmlConverter.Decode(sUTFHtml);
                string sHtmlEncoded;
                for (auto& point : ex)
                {
                    if (point < 128)
                        sHtmlEncoded += (byte)point;
                    else
                        sHtmlEncoded += "&#" + std::to_string(point) + ";";
                }

                std::vector<cHttp::cHeader> headers;
                cHttp::cResponse resp;
                resp.SetResponseCode(cHttp::C_OK);
                resp.SetHeaders(headers);
                resp.SetBody(sHtmlEncoded);
                string resp_str = resp.Serialize();

                aConnections[i]->SendBytes((const byte*)resp_str.c_str(), resp_str.size());

                if (cHttp::GetValueFromHeader(req.GetHeaders(), "connection") != "keep-alive")
                    break;
            }
            else if (status == cNetworkAbstractions::cConnectionStatus::eDISCONNECTED)
            {
                aConnections.erase(aConnections.begin() + i);
                std::cout << "disconnected." << std::endl;
                i--;
                continue;
            }
        }
        sleep(1);
    }
}
