#include <pch.hpp>
#include "NetworkClient.hpp"
#include "NetworkServer.hpp"
#include "Http/HTTP.hpp"
#include <vendor/Utf8.hpp>
#include <DirectoryWatcher.hpp>
#include <vendor/Json.hpp>

#include <openssl/ssl.h>
#include <openssl/err.h>

static string resp_str;

void RequestHandler(NetworkConnection* connection)
{
    pollfd fdarray = { 0 };
    fdarray.fd = connection->sock;
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
            const long size = connection->ReceiveBytes(&buffer[0], 8192);
            if (size <= 0) continue;
            if (size == 0) std::cout << "disconnected" << std::endl;

            const string req_str = string(buffer, size);
            HTTP::Request req = HTTP::Request::deserialize(req_str);

            connection->SendBytes(resp_str.c_str(), resp_str.size());

            string Connection = HTTP::GetValueFromHeader(req.get_headers(), "connection");
            if (Connection != "keep-alive")
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
/*    unsigned char ibuf[] = "compute sha1";
    unsigned char obuf[20];

    SHA1(ibuf, sizeof(ibuf), obuf);

    int i;
    for (i = 0; i < 20; i++) {
        printf("%02x ", obuf[i]);
    }
    printf("\n");*/

    Utf8 utftoHTMLconv;

    std::ifstream oHtmlStream("./wwwroot/index.html");
    if (!oHtmlStream.is_open())
    {
        std::cout << "index.html could not be found." << std::endl;
        return -1;
    }
    string str;
    oHtmlStream >> str;
    auto ex = utftoHTMLconv.Decode(str);
    string html;
    for (auto& point : ex)
    {
        if (point < 128)
            html += (byte)point;
        else
            html += "&#" + std::to_string(point) + ";";
    }

    cDirectoryWatcher dw("wwwroot/", std::chrono::milliseconds(5000));
    threads.push_back(new std::thread([&] {
        dw.Start([&](std::string path_to_watch, cDirectoryWatcher::FileStatus status) -> void {
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
    NetworkConnection::NetworkInitializationSettings settings;
    if (oConfig.empty())
    {
        std::cout << "starting using standard settings..." << std::endl;
        settings.address = "0.0.0.0";
        settings.port = 8080;
    }
    else
    {
        std::cout << "starting using config..." << std::endl;
        settings.address = oConfig["server-settings"]["ip"];
        settings.port = oConfig["server-settings"]["port"];
    }
    settings.ipversion = NetworkConnection::IPVersion::v4;
    settings.connectiontype = NetworkConnection::ConnectionType::TCP;
    settings.mode = NetworkConnection::Mode::NonBlocking;

    NetworkServer server(&settings);
    if (server.Start())
    {
        std::cout << "started on: " << settings.address << ":" << settings.port << std::endl;

        std::vector<HTTP::Header> headers;
        HTTP::Response resp(HTTP::OK, headers, html);
        resp_str = resp.serialize();

        while (true)
        {
            NetworkConnection *incoming = nullptr;
            if ((incoming = server.AcceptConnection(true)) != nullptr)
            {
                std::cout << "New connection from " << inet_ntoa(incoming->addr.sin_addr) << ":"
                          << ntohs(incoming->addr.sin_port) << std::endl;
                threads.push_back(new std::thread(RequestHandler, incoming));
            }
            sleep(1);
        }
    }
    std::cout << "failed to start the server, is the port open?" << std::endl;
    return 0;
}