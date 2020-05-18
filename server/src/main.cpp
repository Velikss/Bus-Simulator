#include <pch.hpp>
#include "GameServer.hpp"

int main()
{
    // initialize server.
    std::cout << "starting...." << std::endl;
    cNetworkConnection::tNetworkInitializationSettings tNetworkSettings;
    tNetworkSettings.sAddress = "0.0.0.0";
    tNetworkSettings.usPort = 8080;
    tNetworkSettings.eMode = cNetworkConnection::cMode::eNonBlocking;

    cGameServer oGameServer(&tNetworkSettings);
    if(oGameServer.Listen())
        std::cout << "game server started." << std::endl;
    else
        std::cout << "game server failed to start." << std::endl;
    for(;;){ fSleep(1000);}
    return 0;
}
