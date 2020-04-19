#pragma once
#include "NetworkConnection.hpp"

class NetworkServer : public NetworkConnection
{
public:
	NetworkServer(NetworkConnection::NetworkInitializationSettings* settings) : NetworkConnection(settings)
	{
	}

	bool Start()
    {
        if ((last_status = bind(sock, (const sockaddr*)&addr, sizeof(addr))) != 0)
        {
            Close();
            return false;
        }
        if ((last_status = listen(sock, SOMAXCONN)) != 0)
        {
            Close();
            return false;
        }
        return true;
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
#if defined(WINDOWS)
			u_long arg = 1;
			ioctlsocket(newSocket, FIONBIO, &arg);
#else
			fcntl(newSocket, F_SETFL, O_NONBLOCK);
#endif
		}
		return new NetworkConnection(newSocket, client_addr);
	}
};
