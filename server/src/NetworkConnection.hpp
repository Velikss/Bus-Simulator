#pragma once
#include <iostream>
#include <string>

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

class NetworkConnection
{
public:
	enum class Mode { Blocking, NonBlocking };
	enum class IPVersion { v4, v6 };
	enum class ConnectionType { TCP, UDP };
	struct NetworkInitializationSettings
	{
		Mode mode;
		IPVersion ipversion;
		ConnectionType connectiontype;
		std::string address;
		unsigned short port;
	};
protected:
	NetworkInitializationSettings* settings = nullptr;
	int last_status = 0;
public:
	sockaddr_in addr{};
	NET_SOCK sock = NET_INVALID_SOCKET_ID;
	// Construction / destructor:
	NetworkConnection(NET_SOCK sock, sockaddr_in addr)
	{
		if (numAliveSockets++ == 0) NetInit();
		this->sock = sock;
		this->addr = addr;
	}
	NetworkConnection(NetworkInitializationSettings* settings)
	{
		if (numAliveSockets++ == 0) NetInit();
		this->settings = settings;

		memset(&addr, 0, sizeof(sockaddr_in));

		addr.sin_family = settings->ipversion == IPVersion::v4 ? AF_INET : AF_INET6;
		addr.sin_addr.s_addr = (settings->address.size() == 0) ? INADDR_ANY : inet_addr(settings->address.c_str());
		addr.sin_port = htons(settings->port);
		
		sock = socket(settings->ipversion == IPVersion::v4 ? AF_INET : AF_INET6, 
					  settings->connectiontype == ConnectionType::TCP ? SOCK_STREAM : SOCK_DGRAM,
						IPPROTO_TCP);

		if (sock == -1 || sock == NET_INVALID_SOCKET_ID)
			throw std::runtime_error("invalid");

		// Optionally set non-blocking.
		// Windows requires a different system call.
		if (settings->mode == Mode::NonBlocking)
		{
#ifdef _WIN32
			u_long arg = 1;
			ioctlsocket(sock, FIONBIO, &arg);
#else // !_WIN32
			fcntl(sock, F_SETFL, O_NONBLOCK);
#endif // _WIN32
		}
	}
	virtual ~NetworkConnection()
	{
		std::cout << "destruct sock" << std::endl;
		Close();
	}

	void Close()
	{
		if (sock != NET_INVALID_SOCKET_ID)
		{
			int result;
#ifdef _WIN32
			result = closesocket(sock);
#else // !_WIN32
			result = close(sock);
#endif // _WIN32
			if (result != 0)
				throw std::runtime_error("Failed to close a socket connection!");
			sock = NET_INVALID_SOCKET_ID;

			if (--numAliveSockets == 0) NetShutdown();
		}
	}

	// Read and write bytes to the socket stream:
	const long ReceiveBytes(char* buffer, size_t numBytes) const
	{
		const long result = recv(sock, buffer, numBytes, 0);
		return result;
	}
	void SendBytes(const char* buffer, size_t numBytes) const
	{
		const long result = send(sock, buffer, numBytes, 0);
		if (result == NET_SOCKET_ERROR)
			throw std::runtime_error("Failed to 'send()' bytes! NET_SOCKET_ERROR!");
		else if (static_cast<size_t>(result) != numBytes)
			throw std::runtime_error("Failed to 'send()' bytes! Couldn't send all the data!");
	}

	// Shared states management.
	// Network is initialized when the fist socket is created
	// and terminated when the last connection is closed.
	static void NetInit()
	{
#ifdef _WIN32
		WSADATA wsa_data;
		WSAStartup(MAKEWORD(1, 1), &wsa_data);
#endif
	}
	static void NetShutdown()
	{
#ifdef _WIN32
		WSACleanup();
#endif
	}
	static unsigned int numAliveSockets;
};
unsigned int NetworkConnection::numAliveSockets = 0;
