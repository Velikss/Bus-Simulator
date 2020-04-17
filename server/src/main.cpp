#include <pch.hpp>
#include "NetworkClient.hpp"
#include "NetworkServer.hpp"
#include "Http/HTTP.hpp"
#include <vendor/Utf8.hpp>
#include <DirectoryWatcher.hpp>

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
            std::cout << "::handled request for: " << req.serialize() << std::endl;

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
    Utf8 utftoHTMLconv;

    FILE* f = nullptr;
    if ((f = fopen("./wwwroot/index.html", "rb")) == NULL)
    {
        std::cout << f << std::endl;
        std::cout << "file does not exist." << std::endl;
        return -1;
    }

    fseek(f, 0L, SEEK_END);
    long long res = ftell(f);
    fseek(f, 0L, SEEK_CUR);
    rewind(f);
    byte* buffer = new byte[res + 1]{ 0 };
    fread(buffer, 1, res, f);
    fclose(f);
    string str((char*)buffer, res);
    auto ex = utftoHTMLconv.Decode(str);
    string html;
    for (auto& point : ex)
    {
        if (point < 128)
            html += (byte)point;
        else
            html += "&#" + std::to_string(point) + ";";
    }
    std::cout << html << std::endl;

   /* cDirectoryWatcher dw("wwwroot/", std::chrono::milliseconds(5000));
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
    }));*/

    NetworkConnection::NetworkInitializationSettings settings;
    settings.address = "0.0.0.0";
    settings.port = 8080;
    settings.ipversion = NetworkConnection::IPVersion::v4;
    settings.connectiontype = NetworkConnection::ConnectionType::TCP;
    settings.mode = NetworkConnection::Mode::NonBlocking;

    NetworkServer server(&settings);
    std::vector<HTTP::Header> headers;
    HTTP::Response resp(HTTP::OK, headers, html);
    resp_str = resp.serialize();

    std::cout << "starting...." << std::endl;
    while (true)
    {
        NetworkConnection* incoming = nullptr;
        if ((incoming = server.AcceptConnection(true)) != nullptr)
        {
            std::cout << "New connection from " << inet_ntoa(incoming->addr.sin_addr) << ":"
                      << ntohs(incoming->addr.sin_port) << std::endl;
            threads.push_back(new std::thread(RequestHandler, incoming));
        }
        sleep(1);
    }
    return 0;
}