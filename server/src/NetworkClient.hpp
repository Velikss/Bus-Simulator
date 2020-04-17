#pragma once
#include "NetworkConnection.hpp"

class NetworkClient : public NetworkConnection
{
public:
	NetworkClient(NetworkConnection::NetworkInitializationSettings* settings) : NetworkConnection(settings)
	{
		if (last_status = connect(sock, (const sockaddr*)&addr, sizeof(addr)) != 0)
		{
			Close();
			throw std::runtime_error("Failed to connect with host " + settings->address +
				" at port #" + std::to_string(settings->port));
		}
	}
};