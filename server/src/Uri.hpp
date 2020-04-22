#pragma once
#include <pch.hpp>

/*
 * Consts for Uri's
 */
const uint C_MAX_URI_SIZE = 131072; // 128 kB
const uint C_MAX_PROTOCOL_CHAR_SIZE = 6;
const uint C_MAX_PORT_CHAR_SIZE = 5;
std::map<string, ushort> C_PROTOCOL_PORT = {
        {"https", 443},
        {"http", 80}
};

/*
 * This class provides parsing of uri strings. It can be used in the server for clear drilldown-references.
 * Supports:
 * - PATHs
 * - GET Parameters non-escaped.
 */
class cUri
{
protected:
    bool valid = false;

    // Used to encode in for example ' ' in '%2'.
    static std::string encode(const std::string &value) noexcept;
    static std::string decode(const std::string &value) noexcept;
public:
    string psProtocol = "";
    string psHost = "";
    ushort pusPort = 0;
    std::vector<string> pasPath;
    std::map<string, string> pasParameters;
    bool IsValidUri() const { return valid; }

    static cUri ParseFromString(string in);

    string ToString();
};

cUri cUri::ParseFromString(string in)
{
    cUri oUri;
    
    in = decode(in);

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
    size_t uiGetParamIndex = in.find('?');

    if (uiPortIndex != string::npos)
    {
        uiPortLastIndex = in.find('/', uiPortIndex);
        // Possible: https://google.com:8080 - without /
        if (uiPortLastIndex == string::npos) uiPortLastIndex = in.size();
        if (uiPortLastIndex - uiPortIndex > C_MAX_PORT_CHAR_SIZE) return oUri;

        // Check for http://google.com:
        if (uiPortLastIndex - uiPortIndex <= 1) return oUri;

        // Check whether there is still a ':' in the uri, this invalidates the uri.
        if (in.find(':', uiPortLastIndex + 1) != string::npos) return oUri;

        // Parse Port
        const cstring pBegin = &in[uiPortIndex] + 1;
        cstring pEnd = &in[uiPortLastIndex];
        try
        {
            oUri.pusPort = (ushort) std::strtoul(pBegin, &pEnd, 0);
        }
        catch (std::exception & ex) // Should the value be too high.
        {
            return oUri;
        }

        uiHostNameEnd = uiPortIndex;
    }
    else
    {
        oUri.pusPort = C_PROTOCOL_PORT.at(sProtocol);
        uiHostNameEnd = in.find('/', uiHostName);

        /*
         * Should the uri be like http://google.com?test=test in which case there is no trailing /
         * Then we can set the ending of the hostname to the first ranked delimiter 1. get, 2. state.
         */
        if (uiHostNameEnd == string::npos)
        {
            if (uiGetParamIndex != string::npos)
                uiHostNameEnd = uiGetParamIndex;
            else
                uiHostNameEnd = in.size();
        }
    }

    oUri.psHost = in.substr(uiHostName, uiHostNameEnd - uiHostName);

    // Try to find a path
    if (uiGetParamIndex != uiHostNameEnd && uiHostNameEnd < in.size())
    {
        size_t uiBegin = uiHostNameEnd;
        size_t uiEnd = string::npos;

        if (in[uiBegin] != '/') uiBegin = in.find('/', uiBegin);

        if (uiBegin != string::npos)
        {
            // Set end of the maximum path
            if (uiGetParamIndex == string::npos) uiEnd = in.size();
            else uiEnd = uiGetParamIndex;

            do
            {
                const size_t uiCurrentBegin = uiBegin + 1;
                uiBegin = in.find('/', uiBegin + 1);
                if (uiCurrentBegin >= uiEnd) break;
                if (uiBegin == string::npos) uiBegin = uiEnd;
                string sParam = in.substr(uiCurrentBegin, uiBegin - uiCurrentBegin);
                if (sParam.size() > 0) oUri.pasPath.push_back(sParam);
            }
            while (uiBegin < uiEnd);
        }
    }

    // Try to find any GET parameters
    if(uiGetParamIndex != string::npos)
    {
        if (uiGetParamIndex < uiHostNameEnd) return oUri;
        do
        {
            // Find whether there is another parameters.
            auto uiCurrentGetParamIndex = uiGetParamIndex;
            uiGetParamIndex = in.find('&', uiGetParamIndex + 1);
            // If not set the end index to the end of the input.
            if (uiGetParamIndex == string::npos) uiGetParamIndex = in.size();

            string sParam = in.substr(uiCurrentGetParamIndex + 1, uiGetParamIndex - uiCurrentGetParamIndex - 1);
            const size_t uiEqualsIndex = sParam.find('=');
            oUri.pasParameters.insert({(string)sParam.substr(0, uiEqualsIndex), (string)sParam.substr(uiEqualsIndex + 1, sParam.size() - uiEqualsIndex)});
        }
        while (uiGetParamIndex < in.size());
    }

    oUri.valid = true;
    return oUri;
}

string cUri::ToString()
{
    string sUri = psProtocol + "://" + psHost;
    if (C_PROTOCOL_PORT.at(psProtocol) != pusPort) sUri += ":" + std::to_string(pusPort);
    for(auto& sSubPath : pasPath)
        sUri += "/" + encode(sSubPath);
    if (pasParameters.size() > 0)
    {
        bool bFirst = true;
        for (auto& [sKey, sValue] : pasParameters)
            if (bFirst)
            {
                sUri += "?" + encode(sKey) + "=" + encode(sValue);
                bFirst = false;
            }
            else
                sUri += "&" + encode(sKey) + "=" + encode(sValue);
    }
    return sUri;
}

std::string cUri::encode(const string &value) noexcept
{
    static auto hex_chars = "0123456789ABCDEF";

    std::string result;
    result.reserve(value.size()); // Minimum size of result

    for(auto &chr : value) {
        if(chr == ' ')
            result += '+';
        else if(chr == '!' || chr == '#' || chr == '$' || (chr >= '&' && chr <= ',') || (chr >= '/' && chr <= ';') || chr == '=' || chr == '?' || chr == '@' || chr == '[' || chr == ']')
            result += std::string("%") + hex_chars[chr >> 4] + hex_chars[chr & 15];
        else
            result += chr;
    }

    return result;
}

std::string cUri::decode(const string &value) noexcept
{
    std::string result;
    result.reserve(value.size() / 3 + (value.size() % 3)); // Minimum size of result

    for(std::size_t i = 0; i < value.size(); ++i) {
        auto &chr = value[i];
        if(chr == '%' && i + 2 < value.size()) {
            auto hex = value.substr(i + 1, 2);
            auto decoded_chr = static_cast<char>(std::strtol(hex.c_str(), nullptr, 16));
            result += decoded_chr;
            i += 2;
        }
        else if(chr == '+')
            result += ' ';
        else
            result += chr;
    }

    return result;
}

