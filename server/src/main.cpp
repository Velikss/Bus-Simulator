#include <pch.hpp>
#include <cNetworkClient.hpp>
#include <cNetworkServer.hpp>
#include <DirectoryWatcher.hpp>
#include <vendor/Json.hpp>
#include <streambuf>
#include <SslHelper.hpp>
#include <filesystem>

string resp_str;

bool OnRecieve(cNetworkConnection* connection)
{
    byte buffer[8192]{0};
    const long size = connection->ReceiveBytes((byte *) &buffer[0], 8192);
    const std::string_view req_str((char*)buffer, size);
    cHttp::cRequest req = cHttp::cRequest::Deserialize((string) req_str);

    connection->SendBytes((const byte*)resp_str.c_str(), resp_str.size());

    return !(cHttp::GetValueFromHeader(req.GetHeaders(), "connection") != "keep-alive");
}

void OnDisConnect(cNetworkConnection* connection)
{
    std::cout << inet_ntoa(connection->ptAddress.sin_addr) << ":"
              << ntohs(connection->ptAddress.sin_port) << ", disconnected." << std::endl;
}

bool OnConnect(cNetworkConnection* connection)
{
    std::cout << "New connection from " << inet_ntoa(connection->ptAddress.sin_addr) << ":"
              << ntohs(connection->ptAddress.sin_port) << std::endl;
    return true;
}

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

    cDirectoryWatcher oDirectoryListener("wwwroot/", std::chrono::milliseconds(5000));
    new std::thread([&] {
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
    });

    // Load config.
    nlohmann::json oConfig;
    std::ifstream oConfigStream("./config.json");
    if (oConfigStream.is_open())
        oConfigStream >> oConfig;

    //init response
    Utf8 oUTF8ToHtmlConverter;
    std::ifstream oHtmlStream("./wwwroot/index.html");
    if (!oHtmlStream.is_open())
    {
        std::cout << "index.html could not be found." << std::endl;
        return false;
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
    resp_str = resp.Serialize();

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
    server.SetOnConnectEvent(OnConnect);
    server.SetOnRecieveEvent(OnRecieve);
    server.SetOnDisconnectEvent(OnDisConnect);
    if (!server.Listen())
        std::cout << "failed to start the server, is the port open?" << std::endl;
    else
        std::cout << "started on: " << tNetworkSettings.sAddress << ":" << tNetworkSettings.usPort << std::endl;
    return 0;
}
