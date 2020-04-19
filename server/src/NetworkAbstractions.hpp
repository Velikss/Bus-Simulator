#pragma once
#include <pch.hpp>

#if defined(WINDOWS)
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
typedef SOCKET NET_SOCK;
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

class cNetworkAbstractions
{
public:
    // Shared states management.
    // Network is initialized when the fist socket is created
    // and terminated when the last connection is closed.
    static void NetInit();
    static void NetShutdown();
    static void SetBlocking(NET_SOCK oSock, bool bBlocking = true);
    static int CloseSocket(NET_SOCK & oSock);
};

void cNetworkAbstractions::NetInit()
{
#if defined(WINDOWS)
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(1, 1), &wsa_data);
#endif
}

void cNetworkAbstractions::NetShutdown()
{
#if defined(WINDOWS)
    WSACleanup();
#endif
}

void cNetworkAbstractions::SetBlocking(NET_SOCK oSock, bool bBlocking)
{
#if defined(WINDOWS)
    u_long arg = (bBlocking) ? 0 : 1;
    ioctlsocket(oSock, FIONBIO, &arg);
#else
    fcntl(oSock, F_SETFL, (bBlocking) ? O_BLOCK : O_NONBLOCK);
#endif
}

int cNetworkAbstractions::CloseSocket(NET_SOCK & oSock)
{
    int result = -1;
#if defined(WINDOWS)
    result = closesocket(oSock);
#else
    result = close(oSock);
#endif
    oSock = NET_INVALID_SOCKET_ID;
    return result;
}

