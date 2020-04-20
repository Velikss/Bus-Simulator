#include <pch.hpp>
#include "cNetworkClient.hpp"
#include "cNetworkServer.hpp"
#include "Http/HTTP.hpp"
#include <vendor/Utf8.hpp>
#include <DirectoryWatcher.hpp>
#include <vendor/Json.hpp>
#include <streambuf>
#include <SslHelper.hpp>
#include <filesystem>

static string resp_str;

void RequestHandler(cNetworkConnection* connection)
{
    pollfd fdarray = {0};
    fdarray.fd = connection->poSock;
    fdarray.events = POLLRDNORM;;
    fdarray.revents = POLLRDNORM;
    while (fdarray.revents & POLLRDNORM)
    {
#ifdef _WIN32
        if (WSAPoll(&fdarray, 1, -1) > 0)
#else
        if (poll(&fdarray, 1, -1) > 0)
#endif
        {
            char buffer[8192];
            const long size = connection->ReceiveBytes((byte *) &buffer[0], 8192);
            if (size < 0) continue;
            if (size == 0) break;
            const string req_str = string(buffer, size);
            HTTP::Request req = HTTP::Request::deserialize(req_str);
            connection->SendBytes((const byte *) resp_str.c_str(), resp_str.size());

            if (HTTP::GetValueFromHeader(req.get_headers(), "connection") != "keep-alive")
                break;
        }
        sleep(1);
    }
    connection->Close();
    std::cout << "disconnected." << std::endl;
}

std::vector<std::thread*> threads;
int main()
{
    cSSLHelper oSSLHelper;

/*    unsigned char ibuf[] = "compute sha1";
    unsigned char obuf[20];

    SHA1(ibuf, sizeof(ibuf), obuf);

    int i;
    for (i = 0; i < 20; i++) {
        printf("%02x ", obuf[i]);
    }
    printf("\n");*/

    Utf8 oUTF8ToHtmlConverter;

    std::ifstream oHtmlStream("./wwwroot/index.html");
    if (!oHtmlStream.is_open())
    {
        std::cout << "index.html could not be found." << std::endl;
        return -1;
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

    std::vector<HTTP::Header> headers;
    HTTP::Response resp(HTTP::OK, headers, sHtmlEncoded);
    resp_str = resp.serialize();

    cDirectoryWatcher oDirectoryListener("wwwroot/", std::chrono::milliseconds(5000));
    threads.push_back(new std::thread([&] {
        oDirectoryListener.Start([&](std::string path_to_watch, cDirectoryWatcher::FileStatus status) -> void {
            // Process only regular files, all other file types are ignored
            if (!std::filesystem::is_regular_file(std::filesystem::path(path_to_watch)) &&
                status != cDirectoryWatcher::FileStatus::erased)
                return;

            switch (status)
            {
                case cDirectoryWatcher::FileStatus::created:
                    std::cout << "File created: " << path_to_watch << '\n';
                    break;
                case cDirectoryWatcher::FileStatus::modified:
                    std::cout << "File modified: " << path_to_watch << '\n';
                    break;
                case cDirectoryWatcher::FileStatus::erased:
                    std::cout << "File erased: " << path_to_watch << '\n';
                    break;
                default:
                    std::cout << "Error! Unknown file status.\n";
            };
        });
    }));

    // Load config.
    nlohmann::json oConfig;
    std::ifstream oConfigStream("./config.json");
    if (oConfigStream.is_open())
        oConfigStream >> oConfig;

    // initialize server.
    std::cout << "starting...." << std::endl;
    cNetworkConnection::tNetworkInitializationSettings tNetworkSettings;
    if (oConfig.empty())
    {
        std::cout << "starting using standard settings..." << std::endl;
        tNetworkSettings.sAddress = "0.0.0.0";
        tNetworkSettings.usPort = 8080;
    }
    else
    {
        std::cout << "starting using config..." << std::endl;
        tNetworkSettings.sAddress = oConfig["server-settings"]["ip"];
        tNetworkSettings.usPort = oConfig["server-settings"]["port"];
        if(oConfig["server-settings"].find("ssl") != oConfig["server-settings"].end())
        {
            if (oConfig["server-settings"]["ssl"].find("cert") != oConfig["server-settings"]["ssl"].end() &&
                oConfig["server-settings"]["ssl"].find("key") != oConfig["server-settings"]["ssl"].end())
            {
                if (is_file_exist(oConfig["server-settings"]["ssl"]["cert"]) &&
                    is_file_exist(oConfig["server-settings"]["ssl"]["key"]))
                {
                    tNetworkSettings.bUseSSL = true;
                    tNetworkSettings.sCertFile = oConfig["server-settings"]["ssl"]["cert"];
                    tNetworkSettings.sKeyFile = oConfig["server-settings"]["ssl"]["key"];
                    std::cout << "security: ssl-enabled." << std::endl;
                }
                else
                    std::cout << "security: no-ssl, one of the files did not exist." << std::endl;
            }
            else
                std::cout << "security: no-ssl, there was a config failure." << std::endl;
        }
        else
            std::cout << "security: no-ssl." << std::endl;
    }
    tNetworkSettings.eIPVersion = cNetworkConnection::cIPVersion::eV4;
    tNetworkSettings.eConnectionType = cNetworkConnection::cConnectionType::eTCP;
    tNetworkSettings.eMode = cNetworkConnection::cMode::eBlocking;

    cNetworkServer server(&tNetworkSettings);
    if (server.Listen())
    {
        std::cout << "started on: " << tNetworkSettings.sAddress << ":" << tNetworkSettings.usPort << std::endl;

        while (true)
        {
            cNetworkConnection *incoming = nullptr;
            if ((incoming = server.AcceptConnection(true)) != nullptr)
            {
                std::cout << "New connection from " << inet_ntoa(incoming->ptAddress.sin_addr) << ":"
                          << ntohs(incoming->ptAddress.sin_port) << std::endl;
                threads.push_back(new std::thread(RequestHandler, incoming));
            }
            sleep(1);
        }
    }
    std::cout << "failed to start the server, is the port open?" << std::endl;
    return 0;
}