#include <pch.hpp>
#include <GameServer/GameServer.hpp>
#include <SSO/SsoServer.hpp>
#include <vendor/Json.hpp>

int main()
{
    std::cout << "Starting Services..." << std::endl;
    nlohmann::json oJson = {};
    std::ifstream oConfigStream("./config.json");
    if (!oConfigStream.is_open()) throw std::runtime_error("could not find config.");
    oConfigStream >> oJson;
    oConfigStream.close();

    if (oJson.empty()) throw std::runtime_error("could not load config.");

    std::shared_ptr<cSSOServer> poSSOServer;
    std::shared_ptr<cGameServer> poGameServer;
    cNetworkConnection::tNetworkInitializationSettings tSSOServerSettings = {};
    cNetworkConnection::tNetworkInitializationSettings tGameServerSettings = {};

    tSSOServerSettings.sAddress = "127.0.0.1";
    tSSOServerSettings.usPort = 14001; //-V525
    tSSOServerSettings.eMode = cNetworkConnection::cMode::eNonBlocking;
    if(oJson.contains("SSOServer"))
    {
        if(oJson["SSOServer"].contains("IP")) tSSOServerSettings.sAddress = oJson["SSOServer"]["IP"];
        if(oJson["SSOServer"].contains("PORT"))
        {
            string sPort = oJson["SSOServer"]["PORT"];
            ushort usPort = strtoul(sPort.c_str(), NULL, 0);
            tSSOServerSettings.usPort = usPort;
        }
        if(oJson["SSOServer"].contains("USESSL")) tSSOServerSettings.bUseSSL = oJson["SSOServer"]["USESSL"];
        if (tSSOServerSettings.bUseSSL)
        {
            if(!oJson["SSOServer"].contains("SSL-CERT") ||
               !oJson["SSOServer"].contains("SSL-KEY"))
                throw std::runtime_error("could not find ssl cert and key settings for the sso server.");
            tSSOServerSettings.sCertFile = oJson["SSOServer"]["SSL-CERT"];
            tSSOServerSettings.sKeyFile = oJson["SSOServer"]["SSL-KEY"];
        }
        if (!oJson["SSOServer"].contains("DB-DRIVER") ||
                !oJson["SSOServer"].contains("DB-DATABASE") ||
                !oJson["SSOServer"].contains("DB-IP") ||
                !oJson["SSOServer"].contains("DB-USR") ||
                !oJson["SSOServer"].contains("DB-PWD"))
            throw std::runtime_error("could not find database settings for the sso server.");
    }
    else
        throw std::runtime_error("could not find database settings for the sso server.");

    tGameServerSettings.sAddress = "0.0.0.0";
    tGameServerSettings.usPort = 14000;
    tGameServerSettings.eMode = cNetworkConnection::cMode::eNonBlocking;
    if(oJson.contains("GameServer"))
    {
        if(oJson["GameServer"].contains("IP")) tGameServerSettings.sAddress = oJson["GameServer"]["IP"];
        if(oJson["GameServer"].contains("PORT"))
        {
            string sPort = oJson["GameServer"]["PORT"];
            ushort usPort = strtoul(sPort.c_str(), NULL, 0);
            tGameServerSettings.usPort = usPort;
        }
        if(oJson["GameServer"].contains("USESSL")) tGameServerSettings.bUseSSL = oJson["GameServer"]["USESSL"];
        if (tGameServerSettings.bUseSSL)
        {
            if(!oJson["GameServer"].contains("SSL-CERT") ||
               !oJson["GameServer"].contains("SSL-KEY"))
                throw std::runtime_error("could not find ssl cert and key settings for the game server.");
            tGameServerSettings.sCertFile = oJson["GameServer"]["SSL-CERT"];
            tGameServerSettings.sKeyFile = oJson["GameServer"]["SSL-KEY"];
        }
        if(!oJson["GameServer"].contains("ID")) throw std::runtime_error("could not find sso id for the game server.");
        if(!oJson["GameServer"].contains("SSO-IP")) throw std::runtime_error("could not find sso ip for the game server.");
        if(!oJson["GameServer"].contains("SSO-PORT")) throw std::runtime_error("could not find sso port for the game server.");

        if (!oJson["GameServer"].contains("DB-DRIVER") ||
            !oJson["GameServer"].contains("DB-DATABASE") ||
            !oJson["GameServer"].contains("DB-IP") ||
            !oJson["GameServer"].contains("DB-USR") ||
            !oJson["GameServer"].contains("DB-PWD"))
            throw std::runtime_error("could not find database settings for the game server.");
    }
    else
        throw std::runtime_error("could not find sso setting for the game server.");

    poSSOServer = std::make_shared<cSSOServer>(&tSSOServerSettings);
    poGameServer = std::make_shared<cGameServer>(&tGameServerSettings);

    if(!poSSOServer->InitDB("driver=" + (string)oJson["SSOServer"]["DB-DRIVER"] + ";" +
                            "server=" + (string)oJson["SSOServer"]["DB-IP"] + ";" +
                            "user=" + (string)oJson["SSOServer"]["DB-USR"] + ";" +
                            "pwd=" + (string)oJson["SSOServer"]["DB-PWD"] + ";" +
                            "database=" + (string)oJson["SSOServer"]["DB-DATABASE"] + ";"))
        throw std::runtime_error("SSO server could not initialize the database.");

    if(oJson["SSOServer"].contains("IDS"))
        for(auto& [iId, sId] : oJson["SSOServer"]["IDS"].items())
        {
            std::cout << "Registering Id: " << sId << ", to the SSO server." << std::endl;
            poSSOServer->RegisterUuid(sId);
        }

    std::cout << "Starting SSOServer..." << std::endl;
    if(!poSSOServer->Listen()) throw std::runtime_error("SSO server could not be started.");
    std::cout << "Started SSOServer at: " << tSSOServerSettings.sAddress << ":" << tSSOServerSettings.usPort << ", SSL: " << (tSSOServerSettings.bUseSSL ? "true" : "false") << std::endl;

    if(!poGameServer->InitDB("driver=" + (string)oJson["GameServer"]["DB-DRIVER"] + ";" +
                             "server=" + (string)oJson["GameServer"]["DB-IP"] + ";" +
                             "user=" + (string)oJson["GameServer"]["DB-USR"] + ";" +
                             "pwd=" + (string)oJson["GameServer"]["DB-PWD"] + ";" +
                             "database=" + (string)oJson["GameServer"]["DB-DATABASE"] + ";"))
        throw std::runtime_error("Game server could not initialize the database.");

    std::cout << "Starting GameServer..." << std::endl;
    fSleep(1000);

    string sPort = oJson["GameServer"]["SSO-PORT"];
    ushort usPort = strtoul(sPort.c_str(), NULL, 0);

    if(!poGameServer->ConnectToSSOServer(oJson["GameServer"]["ID"], oJson["GameServer"]["SSO-IP"], usPort, tGameServerSettings.bUseSSL)) throw std::runtime_error("Game server could connect to the SSO server.");
    if(!poGameServer->Listen()) throw std::runtime_error("Game server could not be started.");

    std::cout << "Started GameServer at: " << tGameServerSettings.sAddress << ":" << tGameServerSettings.usPort << ", SSL: " << (tGameServerSettings.bUseSSL ? "true" : "false") << std::endl;

    for(;;){ fSleep(10000);}
    return 0;
}
