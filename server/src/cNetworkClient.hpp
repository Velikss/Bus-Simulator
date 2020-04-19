#pragma once
#include "cNetworkConnection.hpp"

class cNetworkClient : public cNetworkConnection
{
public:
	explicit cNetworkClient(cNetworkConnection::tNetworkInitializationSettings* ptNetworkSettings) : cNetworkConnection(ptNetworkSettings)
	{
	}

	bool Connect();
};

bool cNetworkClient::Connect()
{
    piLastStatus = connect(poSock, (const sockaddr *) &ptAddress, sizeof(ptAddress)) != 0;
    if (piLastStatus)
    {
        Close();
        return false;
    }
    return true;
}
