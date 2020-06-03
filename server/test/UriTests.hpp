#pragma once
#include <pch.hpp>
#include <gtest/gtest.h>
#include "../src/Uri.hpp"

struct tUriTestCompositon
{
    string sUri;
    string sProtocol;
    string sHost;
    ushort usPort;
    std::vector<string> asPath;
    std::map<string, string> asParameters;
    bool bValid;
};

TEST(UriTests, ParseFromString)
{
    std::vector<tUriTestCompositon> atUris = {
            {"https://google.com",                      "https",    "google.com",       443,    {},    {},    true},
            {"http://google.com",                       "http",     "google.com",       80,     {},    {},    true},
            {"http://google.com:8080",                  "http",     "google.com",       8080,   {},    {},    true},
            {"http://google.com:8080/",                 "http",     "google.com",       8080,   {},    {},    true},
            {"http://google.com:8080/test",             "http",     "google.com",       8080,
                    {
                            "test"
                    },    {},    true},
            {"http://google.com:8080/test/kaas",        "http",     "google.com",       8080,
                    {
                            "test",
                            "kaas"
                    },    {},    true},
            {"http://google.com:8080/test/kaas?h=k",    "http",     "google.com",       8080,
                    {
                            "test",
                            "kaas"
                    },
                    {
                            {"h", "k"}
                    },    true},
            {"http://google.com:8080/t/k?h=k&c=g",      "http",     "google.com",       8080,
                    {
                            "t",
                            "k"
                    },
                    {
                            {"h", "k"},
                            {"c", "g"}
                    },    true},
            {"http://google.com?hallo=hallo",           "http",     "google.com",       80,     {},
                    {
                            {"hallo", "hallo"}
                    }, true},
            {"http://google.com/kaas?hallo=hallo",      "http",     "google.com",       80,
                    {
                            "kaas"
                    },
                    {
                            {"hallo", "hallo"}
                    }, true},
            {"http://google.com/k/s?hallo=hallo",       "http",     "google.com",       80,
                    {
                            "k",
                            "s"
                    },
                    {
                            {"hallo", "hallo"}
                    }, true},
            {"http://google.com/k/s/?hallo=hallo",       "http",     "google.com",       80,
                    {
                            "k",
                            "s"
                    },
                    {
                            {"hallo", "hallo"}
                    }, true},
            {"http://google.com/?hallo=hallo",          "http",     "google.com",       80,    {},
                    {
                            {"hallo", "hallo"}
                    }, true},
            {"http://google.com?hallo=hallo&ik=ben",    "http",     "google.com",       80,    {},
                    {
                            {"hallo", "hallo"},
                            {"ik", "ben"}
                    }, true},
            {"http://google.com/?hallo=hallo&ik=ben",   "http",     "google.com",       80,    {},
                    {
                            {"hallo", "hallo"},
                            {"ik", "ben"}
                    }, true},
            {"http://google.com:/",                     "http",     "",                 0,      {},    {},    false},
            {"http://google.com:",                      "http",     "",                 0,      {},    {},    false},
            {"realestate://google.com",                 "",         "",                 0,      {},    {},    false},
    };
    for (auto& tUri : atUris)
    {
        auto oUri = cUri::ParseFromString(tUri.sUri);
        bool bValid = oUri.IsValidUri();
        EXPECT_EQ(bValid, tUri.bValid);
        if (bValid)
        {
            EXPECT_EQ(oUri.psProtocol, tUri.sProtocol);
            EXPECT_EQ(oUri.psHost, tUri.sHost);
            EXPECT_EQ(oUri.pusPort, tUri.usPort);
            if (tUri.asPath.size() > 0)
            {
                bool bSameSize = tUri.asPath.size() == oUri.pasPath.size();
                EXPECT_TRUE(bSameSize);
                if (bSameSize)
                    for (uint iCounter = 0; iCounter < tUri.asPath.size(); iCounter++) //-V104
                        EXPECT_EQ(oUri.pasPath[iCounter], tUri.asPath[iCounter]); //-V108
            }
            if (tUri.asParameters.size() > 0)
                for (auto& [key, value] : tUri.asParameters)
                {
                    EXPECT_TRUE(oUri.pasParameters.find(key) != oUri.pasParameters.end());
                    EXPECT_TRUE(oUri.pasParameters.at(key) == value);
                }
        }
    }
}

TEST(UriTests, ToString)
{
    cUri oUri;
    oUri.psProtocol = "https";
    oUri.psHost     = "google.com";
    oUri.pusPort    = 443;
    oUri.pasPath.push_back("test");
    oUri.pasPath.push_back("framework");
    oUri.pasParameters.insert({"testParam", "1"});
    oUri.pasParameters.insert({"tester", "false"});
    EXPECT_EQ(oUri.ToString(), "https://google.com/test/framework?testParam=1&tester=false");
}

TEST(UriTests, ParseRequestURI)
{
    string sUri = "/google?hallo=hallo";
    cUri oUri = cUri::ParseFromRequest(sUri);
    EXPECT_TRUE(oUri.IsValidUri());
    EXPECT_TRUE(oUri.pasPath.size() == 1);
    EXPECT_TRUE(oUri.pasPath[0] == "google");
    EXPECT_TRUE(oUri.pasParameters.size() == 1);
    EXPECT_EQ(oUri.pasParameters["hallo"], "hallo");
}