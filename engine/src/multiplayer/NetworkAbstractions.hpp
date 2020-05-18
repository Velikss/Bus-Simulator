#pragma once
#include <pch.hpp>

#if defined(WINDOWS)
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
typedef int NET_SOCK;
#define NET_INVALID_SOCKET_ID INVALID_SOCKET
#define NET_SOCKET_ERROR      SOCKET_ERROR
#elif defined(LINUX)
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<poll.h>
#include <sys/stat.h>
#include <fcntl.h>
typedef int NET_SOCK;
#define NET_INVALID_SOCKET_ID (-1)
#define NET_SOCKET_ERROR      (-1)
#else
#error Unsupported Platform.
#endif
#define MAX_HOSTNAME 1025
#define MAX_SERVNAME 32

class cNetworkAbstractions
{
    static bool pbInit;
public:
    enum class cConnectionStatus
    {
        eDISCONNECTED,
        eCONNECTED,
        eAVAILABLE,
        eCONNECTING
    };

    // Shared states management.
    // Network is initialized when the fist socket is created
    // and terminated when the last connection is closed.
    static void NetInit();
    static void NetShutdown();
    static void SetBlocking(NET_SOCK oSock, bool bBlocking = true);
    static cConnectionStatus IsConnected(NET_SOCK oSock, bool bBlocking);
    static string DNSLookup(string sDomain);
    static string DNSReverseLookup(string sIp);
    static int CloseSocket(NET_SOCK & oSock);
};

bool cNetworkAbstractions::pbInit = false;

void cNetworkAbstractions::NetInit()
{
#if defined(WINDOWS)
    WSADATA tWSA_DATA;
    WSAStartup(MAKEWORD(1, 1), &tWSA_DATA);
#endif
    pbInit = true;
}

void cNetworkAbstractions::NetShutdown()
{
#if defined(WINDOWS)
    WSACleanup();
#endif
    pbInit = false;
}

void cNetworkAbstractions::SetBlocking(NET_SOCK oSock, bool bBlocking)
{
#if defined(WINDOWS)
    u_long ulArgument = (bBlocking) ? 0 : 1;
    ioctlsocket(oSock, FIONBIO, &ulArgument);
#else
      const int flags = fcntl(oSock, F_GETFL, 0);
    fcntl(oSock, F_SETFL, bBlocking ? flags ^ O_NONBLOCK : flags | O_NONBLOCK);
#endif
}

int cNetworkAbstractions::CloseSocket(NET_SOCK & oSock)
{
    int iResult = -1;

#ifdef _WIN32
    shutdown(oSock, SD_BOTH);
    iResult = closesocket(oSock);
#else
    shutdown(oSock, SHUT_RDWR);
    iResult = close(oSock);
#endif

    oSock = NET_INVALID_SOCKET_ID;
    return iResult;
}

cNetworkAbstractions::cConnectionStatus cNetworkAbstractions::IsConnected(NET_SOCK oSock, bool bBlocking)
{
    char pBuffer;
    int size = recv(oSock, &pBuffer, 1, MSG_PEEK);
#if defined(WINDOWS)
    int err = WSAGetLastError();
    if (err == WSAECONNRESET)
        return cNetworkAbstractions::cConnectionStatus::eDISCONNECTED;
#else
    if (size == 0) return cNetworkAbstractions::cConnectionStatus::eDISCONNECTED;
#endif
    else if (size > 0) return cNetworkAbstractions::cConnectionStatus::eAVAILABLE;
    else return cNetworkAbstractions::cConnectionStatus::eCONNECTED;
}

string cNetworkAbstractions::DNSLookup(string sDomain)
{
    if (!pbInit) NetInit();
    addrinfo *result;
    if(getaddrinfo(sDomain.c_str(), NULL, NULL, &result) != 0)
        return "127.0.0.1";
    return string(inet_ntoa(((sockaddr_in*)result->ai_addr)->sin_addr));
}

string cNetworkAbstractions::DNSReverseLookup(string sIp)
{
    sockaddr_in saGNI;
    char hostname[MAX_HOSTNAME];
    char servInfo[MAX_SERVNAME];
    u_short port = 53;

    saGNI.sin_family = AF_INET;
    saGNI.sin_addr.s_addr = inet_addr(sIp.c_str());
    saGNI.sin_port = htons(port);

    if(getnameinfo((struct sockaddr *) &saGNI,
                           sizeof (struct sockaddr),
                           hostname,
                           NI_MAXHOST, servInfo, NI_MAXSERV, NI_NUMERICSERV) != 0)
        return "";
    return string(hostname);
}
