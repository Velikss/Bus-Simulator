#pragma once
#include <iostream>
#include <string>
#include <NetworkAbstractions.hpp>

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

	NetworkConnection(NET_SOCK sock, sockaddr_in addr)
	{
		if (numAliveSockets++ == 0) cNetworkAbstractions::NetInit();
		this->sock = sock;
		this->addr = addr;
	}

	NetworkConnection(NetworkInitializationSettings* settings)
	{
		if (numAliveSockets++ == 0) cNetworkAbstractions::NetInit();
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
		    cNetworkAbstractions::SetBlocking(sock, false);
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
			if(cNetworkAbstractions::CloseSocket(sock) != 0)
			    throw std::runtime_error("could not close socket");

			if (--numAliveSockets == 0) cNetworkAbstractions::NetShutdown();
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

	static unsigned int numAliveSockets;
};
unsigned int NetworkConnection::numAliveSockets = 0;
