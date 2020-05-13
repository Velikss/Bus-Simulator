#include <pch.hpp>
#include <cNetworkClient.hpp>
#include <cNetworkServer.hpp>
#include <streambuf>
#include <filesystem>

void RecieveData(cNetworkConnection* pConnection, byte* & buffer, int & iRecievedContent)
{
    int iSize = 0;
    const long recievedSize = pConnection->ReceiveBytes(((byte *) &iSize), 4);
    if (recievedSize != 4) std::cout << "didn't recieve header." << std::endl;
    buffer = new byte[iSize];
    std::cout << "recieving: " << iSize << std::endl;
    while (iRecievedContent != iSize)
    {
        iRecievedContent += pConnection->ReceiveBytes(buffer + iRecievedContent, iSize - iRecievedContent);
        std::cout << "recieved: " << iRecievedContent << ", out of: " << iSize << std::endl;
    }
}

void SendData(cNetworkConnection* pConnection, byte* buffer, int iSize)
{
    pConnection->SendBytes((byte*)&iSize, 4);
    pConnection->SendBytes(buffer, iSize);
}

void SendData(cNetworkClient* pConnection, byte* buffer, int iSize)
{
    pConnection->SendBytes((byte*)&iSize, 4);
    pConnection->SendBytes(buffer, iSize);
}

bool OnRecieve(cNetworkConnection* pConnection)
{
    std::cout << "recieving data..." << std::endl;
    byte* buffer = nullptr;
    int iRecievedContent = 0;
    RecieveData(pConnection, buffer, iRecievedContent);
    std::string_view sBuffer((char*)buffer, iRecievedContent);
    std::cout << sBuffer << std::endl;
    return true;
}

void OnDisConnect(cNetworkConnection* connection)
{
    std::cout << connection->GetConnectionString() << ", disconnected." << std::endl;
}

bool OnConnect(cNetworkConnection* connection)
{
    std::cout << "New connection from " << connection->GetConnectionString() << std::endl;
    return true;
}

int main()
{
    // initialize server.
    std::cout << "starting...." << std::endl;
    cNetworkConnection::tNetworkInitializationSettings tNetworkSettings;
    std::cout << "starting using standard settings..." << std::endl;
    tNetworkSettings.sAddress = "0.0.0.0";
    tNetworkSettings.usPort = 8080;
    tNetworkSettings.eMode = cNetworkConnection::cMode::eNonBlocking;

    cNetworkServer server(&tNetworkSettings);
    server.SetOnConnectEvent(OnConnect);
    server.SetOnRecieveEvent(OnRecieve);
    server.SetOnDisconnectEvent(OnDisConnect);
    if (!server.Listen())
        std::cout << "failed to start the server, is the port open?" << std::endl;
    else
        std::cout << "started on: " << tNetworkSettings.sAddress << ":" << tNetworkSettings.usPort << std::endl;

    cNetworkConnection::tNetworkInitializationSettings tConnectNetworkSettings;
    std::cout << "connecting using standard settings..." << std::endl;
    tConnectNetworkSettings.sAddress = "127.0.0.1";
    tConnectNetworkSettings.usPort = 8080;
    tConnectNetworkSettings.eMode = cNetworkConnection::cMode::eNonBlocking;

    cNetworkClient client(&tConnectNetworkSettings);
    client.SetOnConnectEvent([&](cNetworkConnection* connection) -> void{
        std::cout << "connected client." << std::endl;
    });
    client.SetOnDisconnectEvent([&](cNetworkConnection* connection) -> void{
        std::cout << "disconnected client." << std::endl;
    });
    client.SetOnRecieveEvent([&](cNetworkConnection* pConnection) -> bool{
        std::cout << "client recieving data..." << std::endl;
        byte* buffer = nullptr;
        int iRecievedContent = 0;
        RecieveData(pConnection, buffer, iRecievedContent);
        std::string_view sBuffer((char*)buffer, iRecievedContent);
        std::cout << sBuffer << std::endl;
        return true;
    });
    if (!client.Connect()) std::cout << "failed to connect." << std::endl;
    else
    {
        string sRequest = "hallo ik ben bram.";
        SendData(&client, (byte*)sRequest.c_str(), sRequest.size());
    }

    for(;;){sleep(1000);}
    return 0;
}
