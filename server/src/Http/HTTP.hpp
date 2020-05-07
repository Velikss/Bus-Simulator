#pragma once
#include <pch.hpp>

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

        const string &GetKey() noexcept
        {
            return this->psKey;
        }

        string Serialize() const noexcept
        {
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

        string GetHeader(string sKey)
        {
            for (auto &oHeader : paHeaders)
                if (oHeader.GetKey() == sKey)
                    return oHeader.GetValue();
            return "";
        }

        const string &GetBody()
        {
            return this->psBody;
        }
        void SetBody(string sBody)
        {
            this->psBody = sBody;
        }

        const cVersion &GetVersion()
        {
            return this->peVersion;
        }
        void SetVersion(cVersion &eVersion)
        {
            this->peVersion = eVersion;
        }

        string Serialize()
        {
            string sMessage;
            SerializeMeta(sMessage);
            SerializeContent(sMessage);
            return sMessage;
        }

        virtual void SerializeMeta(string & sTarget)
        {
            // Stub
        }

        void SerializeContent(string& sTarget)
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
    };

    class cRequest : public cHttpMessage
    {
        string psResource;
        cMethod peMethod = cMethod::eGET;

        static long GetContentLengthOfString(const std::string_view& sRequest, cRequest & oRequest, size_t & lBodyBegin)
        {
            string sContentLength = oRequest.GetHeader("content-length");
            if (sContentLength.size() == 0) return 0;
            return std::stoi(sContentLength);
        }
    public:
        string &GetResource()
        {
            return this->psResource;
        }
        void SetResource(string sResource)
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

        virtual void SerializeMeta(string & sTarget) override
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
            if(lBodyBegin == string::npos) throw std::runtime_error("could not find end of meta.");
            std::string sMeta(sRequest.data(), lBodyBegin);
            std::vector<string> aLines = split((string)sMeta, C_LINE_END);

            std::vector<string> aMetaSegments = split(aLines[0], " ");

            cMethod eMethod = aMethodStrToMethod.at(aMetaSegments[0]);
            string sResource = aMetaSegments[1];
            cVersion eVersion = aVersionStrToVersion.at(aMetaSegments[2]);

            std::vector<cHeader> aHeaders;
            for (size_t i = 1; i < aLines.size(); i++)
                aHeaders.push_back(cHeader::Deserialize(aLines[i]));

            oRequest.SetMethod(eMethod);
            oRequest.SetResource(sResource);
            oRequest.SetHeaders(aHeaders);
            oRequest.SetVersion(eVersion);
            long iContentLength = GetContentLengthOfString(sRequest, oRequest, lBodyBegin);
            oRequest.SetMissingContent(sRequest.size() - (lBodyBegin + iContentLength + 4));
        }

        static void DeserializeContent(const std::string_view& sRequest, cRequest & oRequest)
        {
            size_t lBodyBegin = 0;
            long iContentLength = GetContentLengthOfString(sRequest, oRequest, lBodyBegin);
            oRequest.SetMissingContent(sRequest.size() - (lBodyBegin + 4 + iContentLength));
            oRequest.SetBody(string(sRequest, lBodyBegin, iContentLength));
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
        unsigned short pusResponseCode;
    public:
        const unsigned short &GetResponseCode()
        {
            return this->pusResponseCode;
        }
        void SetResponseCode(unsigned short &usResponseCode)
        {
            this->pusResponseCode = usResponseCode;
        }

        virtual void SerializeMeta(string & sTarget) override
        {
            sTarget += aVersionToVersionStr.at(this->peVersion);
            sTarget += " ";
            sTarget += std::to_string(this->pusResponseCode);
            sTarget += C_LINE_END;
        }

        static cResponse Deserialize(const string &sResponse)
        {
            std::vector<string> aSections = split(sResponse, C_LINE_END + C_LINE_END);
            std::vector<string> aLines = split(aSections[0], C_LINE_END);

            std::vector<string> aMetaSegments = split(aLines[0], " ");
            cVersion eVersion = aVersionStrToVersion.at(aMetaSegments[0]);

            std::vector<cHeader> aHeaders;
            for (size_t i = 1; i < aLines.size(); i++)
                aHeaders.push_back(cHeader::Deserialize(aLines[i]));

            string sBody;
            if(aSections.size() > 1)
                sBody = concat(aSections, "", 1);

            unsigned short usResponseCode = 0;
            const char* pBegin = aMetaSegments[1].c_str();
            char* pEnd = (char*)aMetaSegments[1].c_str() + aMetaSegments[1].size() - 1;
            usResponseCode = (unsigned short) std::strtoul(pBegin, &pEnd, 0);

            cResponse oResponse;
            oResponse.SetResponseCode(usResponseCode);
            oResponse.SetHeaders(aHeaders);
            oResponse.SetVersion(eVersion);
            oResponse.SetBody(sBody);

            return oResponse;
        }
    };
}
