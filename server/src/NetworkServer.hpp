#pragma once
#include "NetworkConnection.hpp"

class NetworkServer : public NetworkConnection
{
public:
	NetworkServer(NetworkConnection::NetworkInitializationSettings* settings) : NetworkConnection(settings)
	{
		if ((last_status = bind(sock, (const sockaddr*)&addr, sizeof(addr))) != 0)
		{
			Close();
			throw std::runtime_error("Failed to bind to host " + settings->address +
				" at port #" + std::to_string(settings->port));
		}
		std::cout << last_status << std::endl;
		if ((last_status = listen(sock, SOMAXCONN)) != 0)
		{
			Close();
			throw std::runtime_error("Failed to bind to host " + settings->address +
				" at port #" + std::to_string(settings->port));
		}
        std::cout << last_status << std::endl;
	}

	NetworkConnection* AcceptConnection(bool setnonblocking = false) const
	{
		sockaddr_in client_addr;
		memset(&client_addr, 0, sizeof(sockaddr_in));
		socklen_t client_addr_len = sizeof(client_addr);
		const NET_SOCK newSocket = accept(sock, (struct sockaddr*) & client_addr,
			&client_addr_len);

		if (newSocket == -1)
			return nullptr;
		if (setnonblocking)
		{
#ifdef _WIN32
			u_long arg = 1;
			ioctlsocket(newSocket, FIONBIO, &arg);
#else // !_WIN32
			fcntl(newSocket, F_SETFL, O_NONBLOCK);
#endif // _WIN32
		}
		return new NetworkConnection(newSocket, client_addr);
	}
};
