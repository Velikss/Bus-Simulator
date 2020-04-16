#pragma once
#include <pch.hpp>

/*
 * Consts for Uri's
 */
const uint C_MAX_URI_SIZE = 131072; // 128 kB
const uint C_MAX_PROTOCOL_CHAR_SIZE = 6;
const uint C_MAX_PORT_CHAR_SIZE = 5;
const uint C_MAX_PORT_SIZE = 65535;
std::map<string, ushort> C_PROTOCOL_PORT = {
        {"https", 443},
        {"http", 80}
};

/*
 * This class provides parsing of uri strings. It can be used in the server for clear drilldown-references.
 */
class cUri
{
protected:
    bool valid = false;
public:
    string psProtocol = "";
    string psHost = "";
    ushort pusPort = 0;
    bool IsValidUri() const { return valid; }
    static cUri ParseFromString(string in);
};

cUri cUri::ParseFromString(string in)
{
    cUri oUri;

    // Unsupported '\'
    if (in.find('\\') != string::npos) return oUri;

    // Test whether the input > MAXIMUM_URI_SIZE
    if (in.size() > C_MAX_URI_SIZE) return oUri;

    // First check the protocol type.
    const size_t uiProtocolLastIndex = in.find("://");
    if (uiProtocolLastIndex == string::npos) return oUri;
    if (uiProtocolLastIndex >= C_MAX_PROTOCOL_CHAR_SIZE) return oUri;

    string sProtocol = in.substr(0, uiProtocolLastIndex);
    if (C_PROTOCOL_PORT.find(sProtocol) == C_PROTOCOL_PORT.end()) return oUri;
    oUri.psProtocol = sProtocol;

    // Initialize a hostname.
    const size_t uiHostName = uiProtocolLastIndex + 3;
    size_t uiHostNameEnd = string::npos;

    // Check if there is a port specified.
    const size_t uiPortIndex = in.find(':', uiProtocolLastIndex + 1);
    size_t uiPortLastIndex = string::npos;

    // Check if there is a GET parameter.
    const size_t uiGetParamIndex = in.find('?');

    if (uiPortIndex != string::npos)
    {
        uiPortLastIndex = in.find('/', uiPortIndex);
        // Possible: https://google.com:8080 - without /
        if (uiPortLastIndex == string::npos) uiPortLastIndex = in.size();
        if (uiPortLastIndex - uiPortIndex > C_MAX_PORT_CHAR_SIZE) return oUri;

        // Check whether there is still a ':' in the uri, this invalidates the uri.
        if (in.find(':', uiPortLastIndex + 1) != string::npos) return oUri;

        // Parse Port
        const cstring pBegin = &in[uiPortIndex] + 1;
        cstring pEnd = &in[uiPortLastIndex];
        oUri.pusPort = (ushort) std::strtoul(pBegin, &pEnd, 0);

        uiHostNameEnd = uiPortIndex;
    }
    else
    {
        oUri.pusPort = C_PROTOCOL_PORT.at(sProtocol);
        uiHostNameEnd = in.find('/', uiHostName);
    }

    oUri.psHost = in.substr(uiHostName, uiHostNameEnd - uiHostName);

/*    // Try to find any GET parameters
    if(uiGetParamIndex != string::npos)
    {
        if (uiGetParamIndex < uiHostNameEnd)
    }*/

    oUri.valid = true;
    return oUri;
}

