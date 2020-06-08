#pragma once
#include <pch.hpp>
#include <NetworkConnection.hpp>

namespace cHttp
{
    const string C_LINE_END = "\r\n";
    enum class cMethod
    {
        eGET,
        eHEAD,
        ePOST,
        ePUT,
        eDEL,
        eTRACE,
        eOPTIONS,
        eCONNECT,
        ePATCH
    };
    enum class cVersion
    {
        eHTTP_1_0,
        eHTTP_1_1,
        eHTTP_2_0
    };
    std::map<cVersion, string> aVersionToVersionStr
            {
                    {cVersion::eHTTP_2_0, "HTTP/2.0"},
                    {cVersion::eHTTP_1_1, "HTTP/1.1"},
                    {cVersion::eHTTP_1_0, "HTTP/1.0"},
            };
    std::map<string, cVersion> aVersionStrToVersion
            {
                    {"HTTP/2.0", cVersion::eHTTP_2_0},
                    {"HTTP/1.1", cVersion::eHTTP_1_1},
                    {"HTTP/1.0", cVersion::eHTTP_1_0},
            };
    std::map<cMethod, string> aMethodToMethodStr
            {
                    {cMethod::eGET,     "GET"},
                    {cMethod::eHEAD,    "HEAD"},
                    {cMethod::ePOST,    "POST"},
                    {cMethod::ePUT,     "PUT"},
                    {cMethod::eDEL,     "DELETE"},
                    {cMethod::eTRACE,   "TRACE"},
                    {cMethod::eOPTIONS, "OPTIONS"},
                    {cMethod::eCONNECT, "CONNECT"},
                    {cMethod::ePATCH,   "PATCH"},
            };
    std::map<string, cMethod> aMethodStrToMethod
            {
                    {"GET",     cMethod::eGET},
                    {"HEAD",    cMethod::eHEAD},
                    {"POST",    cMethod::ePOST},
                    {"PUT",     cMethod::ePUT},
                    {"DELETE",  cMethod::eDEL},
                    {"TRACE",   cMethod::eTRACE},
                    {"OPTIONS", cMethod::eOPTIONS},
                    {"CONNECT", cMethod::eCONNECT},
                    {"PATCH",   cMethod::ePATCH},
            };

    static unsigned short C_OK                      = 200;
    static unsigned short C_CREATED                 = 201;
    static unsigned short C_ACCEPTED                = 202;
    static unsigned short C_NO_CONTENT              = 203;
    static unsigned short C_MOVED                   = 301;
    static unsigned short C_REDIRECT                = 307;
    static unsigned short C_BAD_REQUEST             = 400;
    static unsigned short C_FORBIDDEN               = 403;
    static unsigned short C_NOT_FOUND               = 404;
    static unsigned short C_REQUEST_TIMEOUT         = 408;
    static unsigned short C_INTERNAL_SERVER_ERROR   = 500;
    static unsigned short C_BAD_GATEWAY             = 502;
    static unsigned short C_SERVICE_UNAVAILABLE     = 503;

    class cHeader
    {
        string psKey;
        string psValue;
    public:
        cHeader(const string &sKey, const string &sValue) noexcept: psKey(sKey), psValue(sValue)
        {
        }

        void SetValue(const string &sValue) noexcept
        {
            this->psValue = sValue;
        }

        const string &GetValue() const noexcept
        {
            return this->psValue;
        }

        void SetKey(const string &sKey) noexcept
        {
            this->psKey = sKey;
        }

        const string &GetKey() const noexcept
        {
            return this->psKey;
        }

        string Serialize() const noexcept
        {
            if (this->psValue.size() == 0) return "";

            string sHeader;
            sHeader += this->psKey;
            sHeader += ": ";
            sHeader += this->psValue;
            sHeader += C_LINE_END;

            return sHeader;
        }

        static cHeader Deserialize(const string &sHeader)
        {
            size_t uiPos = sHeader.find(':');
            if (uiPos == string::npos)
            {
                std::cout << "invalid header" << std::endl;;
                return cHeader("", "");
            }
            string sKey(sHeader.substr(0, uiPos));
            toLower(sKey);

            size_t uiFirst = sHeader.find_first_not_of(" \t", uiPos + 1);
            size_t uiLast = sHeader.find_last_not_of(" \t");
            string sValue(sHeader.substr(uiFirst, uiLast - uiFirst + 1));
            toLower(sValue);

            return cHeader(sKey, sValue);
        }
    };

    string GetValueFromHeader(std::vector<cHeader> &aHeaders, const string &sKey)
    {
        for (auto &header : aHeaders)
            if (header.GetKey() == sKey)
                return header.GetValue();
        return "";
    }

    class cHttpMessage
    {
    protected:
        cVersion peVersion = cVersion::eHTTP_1_1;
        std::vector<cHeader> paHeaders;
        string psBody;
        size_t plMissingContent = 0;
        size_t piMetaLength = 0;

        static long GetContentLengthOfString(const std::string_view& sMessage, cHttpMessage & oMessage, size_t & lEndMeta)
        {
            string sContentLength = oMessage.GetHeader("content-length");
            lEndMeta = sMessage.find(C_LINE_END + C_LINE_END);
            auto lEndRequest = sMessage.find(C_LINE_END + C_LINE_END, lEndMeta + 4);
            if (sContentLength.size() == 0) return 0;
            return std::stoi(sContentLength);
        }
    public:
        size_t& GetMissingContent()
        {
            return this->plMissingContent;
        }
        void SetMissingContent(size_t lMissingContent)
        {
            this->plMissingContent = lMissingContent;
        }

        std::vector<cHeader> &GetHeaders()
        {
            return this->paHeaders;
        }
        void SetHeaders(std::vector<cHeader> &aHeaders)
        {
            this->paHeaders = aHeaders;
        }

        string GetHeader(const string& sKey)
        {
            for (auto &oHeader : paHeaders)
                if (oHeader.GetKey() == sKey)
                    return oHeader.GetValue();
            return "";
        }
        void SetHeader(const string& sKey, const string& sValue)
        {
            for (auto &oHeader : paHeaders)
                if (oHeader.GetKey() == sKey)
                {
                    oHeader.SetValue(sValue);
                    return;
                }
            paHeaders.push_back({sKey, sValue});
        }

        const string &GetBody()
        {
            return this->psBody;
        }
        void SetBody(const string& sBody)
        {
            this->psBody = sBody;
        }

        const cVersion &GetVersion()
        {
            return this->peVersion;
        }
        void SetVersion(const cVersion &eVersion) //-V669
        {
            this->peVersion = eVersion;
        }

        string Serialize() const
        {
            string sMessage;
            SerializeMeta(sMessage);
            SerializeContent(sMessage);
            return sMessage;
        }

        virtual void SerializeMeta(string & sTarget) const
        {
            // Stub
        }

        void SerializeContent(string& sTarget) const
        {
            for (auto &oHeader : this->paHeaders)
                if(oHeader.GetKey() != "content-length")
                    sTarget += oHeader.Serialize();

            if (psBody.size())
                sTarget += cHeader("content-length", std::to_string(psBody.size())).Serialize();

            sTarget += C_LINE_END;
            if (psBody.size() > 0)
                sTarget += psBody;
        }

        static void DeserializeContent(const std::string_view& sMessage, cHttpMessage & oMessage)
        {
            size_t lBodyBegin = 0;
            long iContentLength = GetContentLengthOfString(sMessage, oMessage, lBodyBegin);
            oMessage.SetMissingContent(sMessage.size() - (lBodyBegin + 4 + iContentLength)); //-V112 //-V104
            oMessage.SetBody(string(sMessage, lBodyBegin, iContentLength)); //-V106
        }

        size_t &GetMetaLength()
        {
            return this->piMetaLength;
        }
    };

    class cRequest : public cHttpMessage
    {
        string psResource;
        cMethod peMethod = cMethod::eGET;
    public:
        string &GetResource()
        {
            return this->psResource;
        }
        void SetResource(const string& sResource)
        {
            this->psResource = sResource;
        }

        const cMethod& GetMethod()
        {
            return this->peMethod;
        }
        void SetMethod(cMethod eMethod)
        {
            this->peMethod = eMethod;
        }

        virtual void SerializeMeta(string & sTarget) const override
        {
            sTarget += aMethodToMethodStr.at(this->peMethod);
            sTarget += " ";
            sTarget += this->psResource;
            sTarget += " ";
            sTarget += aVersionToVersionStr.at(this->peVersion);
            sTarget += C_LINE_END;
        }

        static void DeserializeMeta(const std::string_view& sRequest, cRequest & oRequest)
        {
            string sBodySplit = C_LINE_END + C_LINE_END;
            size_t lBodyBegin = sRequest.find(sBodySplit);

            oRequest.piMetaLength = lBodyBegin + 4; //-V112

            std::string sMeta(sRequest.data(), lBodyBegin);
            std::vector<string> aLines = split((string)sMeta, C_LINE_END);

            std::vector<string> aMetaSegments = split(aLines[0], " ");

            cMethod eMethod = aMethodStrToMethod.at(aMetaSegments[0]);
            string sResource = aMetaSegments[1];
            cVersion eVersion = aVersionStrToVersion.at(aMetaSegments[2]);

            std::vector<cHeader> aHeaders;
            for (size_t i = 1; i < aLines.size(); i++)
                aHeaders.emplace_back(cHeader::Deserialize(aLines[i]));

            oRequest.SetMethod(eMethod);
            oRequest.SetResource(sResource);
            oRequest.SetHeaders(aHeaders);
            oRequest.SetVersion(eVersion);
            long lContentLength = GetContentLengthOfString(sRequest, oRequest, lBodyBegin);
            oRequest.SetMissingContent(sRequest.size() - (lBodyBegin + lContentLength + 4)); //-V112 //-V104
        }

        static cRequest Deserialize(const std::string_view & sRequest)
        {
            cRequest oRequest;

            DeserializeMeta(sRequest, oRequest);
            DeserializeContent(sRequest, oRequest);

            return oRequest;
        }
    };

    class cResponse : public cHttpMessage
    {
        unsigned short pusResponseCode = 0;
    public:
        const unsigned short &GetResponseCode()
        {
            return this->pusResponseCode;
        }
        void SetResponseCode(const unsigned short &usResponseCode)
        {
            this->pusResponseCode = usResponseCode;
        }

        virtual void SerializeMeta(string & sTarget) const override
        {
            sTarget += aVersionToVersionStr.at(this->peVersion);
            sTarget += " ";
            sTarget += std::to_string(this->pusResponseCode);
            sTarget += C_LINE_END;
        }

        static void DeserializeMeta(const std::string_view& sRequest, cResponse & oResponse)
        {
            string sBodySplit = C_LINE_END + C_LINE_END;
            size_t lBodyBegin = sRequest.find(sBodySplit);

            oResponse.piMetaLength = lBodyBegin + 4; //-V112

            std::string sMeta(sRequest.data(), lBodyBegin);
            std::vector<string> aLines = split((string)sMeta, C_LINE_END);

            std::vector<string> aMetaSegments = split(aLines[0], " ");
            cVersion eVersion = aVersionStrToVersion.at(aMetaSegments[0]);

            unsigned short usResponseCode = 0;
            const char* pBegin = aMetaSegments[1].c_str();
            char* pEnd = (char*)aMetaSegments[1].c_str() + aMetaSegments[1].size() - 1;
            usResponseCode = (unsigned short) std::strtoul(pBegin, &pEnd, 0);

            std::vector<cHeader> aHeaders;
            for (size_t i = 1; i < aLines.size(); i++)
                aHeaders.emplace_back(cHeader::Deserialize(aLines[i]));

            long lContentLength = GetContentLengthOfString(sRequest, oResponse, lBodyBegin);
            oResponse.SetMissingContent(sRequest.size() - (lBodyBegin + lContentLength + 4)); //-V112 //-V104

            oResponse.SetResponseCode(usResponseCode);
            oResponse.SetHeaders(aHeaders);
            oResponse.SetVersion(eVersion);
        }

        static cResponse Deserialize(const std::string_view & sResponse)
        {
            cResponse oResponse;

            DeserializeMeta(sResponse, oResponse);
            DeserializeContent(sResponse, oResponse);

            return oResponse;
        }
    };

    bool RecieveRequest(cNetworkConnection* pConnection, cRequest& oRequest, int iTimeOut = 300)
    {
        while (!pConnection->Available() && (iTimeOut > 0 || iTimeOut == -1))
        {
            fSleep(50);
            if (iTimeOut != -1) iTimeOut-=50;
        }
        if (!pConnection->Available()) return false;

        byte* aBytes = new byte[8192];
        size_t size = pConnection->ReceiveBytes(aBytes, 8192); //-V101
        std::string_view sBytes((const char*)aBytes, size);
        cHttp::cRequest::DeserializeMeta(sBytes, oRequest);

        int iContentLength = std::atoi(oRequest.GetHeader("content-length").c_str());
        size_t iRequestLength = iContentLength + oRequest.GetMetaLength(); //-V104
        if (iRequestLength >= 8192)
        {
            byte* aNewBuffer = new byte[iRequestLength];
            memcpy(aNewBuffer, aBytes, size);
            delete[] aBytes;
            aBytes = aNewBuffer;
            sBytes = std::string_view((const char*)aBytes, size);
        }

        cHttp::cRequest::DeserializeContent(sBytes, oRequest);

        size_t lMissingContent = 0;
        while ((lMissingContent = oRequest.GetMissingContent()) != 0)
        {
            size += (size_t) pConnection->ReceiveBytes(aBytes + size, (int) lMissingContent);
            cHttp::cRequest::DeserializeContent(sBytes, oRequest);
        }

        return true;
    }

    bool RecieveResponse(cNetworkConnection* pConnection, cResponse& oResponse, int uiTimeOut = 300)
    {
        while (!pConnection->Available() && (uiTimeOut > 0 || uiTimeOut == -1))
        {
            fSleep(50);
            if (uiTimeOut != -1) uiTimeOut-=50;
        }
        if (!pConnection->Available()) return false;

        byte* aBytes = new byte[8192];
        size_t size = pConnection->ReceiveBytes(aBytes, 8192); //-V101
        std::string_view sBytes((const char*)aBytes, size);
        cHttp::cResponse::DeserializeMeta(sBytes, oResponse);

        int iContentLength = std::atoi(oResponse.GetHeader("content-length").c_str());
        size_t iRequestLength = iContentLength + oResponse.GetMetaLength(); //-V104
        if (iRequestLength >= 8192)
        {
            byte* aNewBuffer = new byte[iRequestLength];
            memcpy(aNewBuffer, aBytes, size);
            delete[] aBytes;
            aBytes = aNewBuffer;
            sBytes = std::string_view((const char*)aBytes, size);
        }

        cHttp::cResponse::DeserializeContent(sBytes, oResponse);

        size_t lMissingContent = 0;
        while ((lMissingContent = oResponse.GetMissingContent()) != 0)
        {
            size += (size_t) pConnection->ReceiveBytes(aBytes + size, (int) lMissingContent);
            cHttp::cResponse::DeserializeContent(sBytes, oResponse);
        }

        return true;
    }
}
