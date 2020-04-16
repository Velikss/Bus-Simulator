#pragma once
#include <pch.hpp>
#include <gtest/gtest.h>
#include <server/src/Uri.hpp>

struct tUriTestCompositon
{
    string sUri;
    string sProtocol;
    string sHost;
    ushort usPort;
    std::map<string, string> asParameters;
    bool bValid;
};

TEST(UriTests, ParseFromString)
{
    std::vector<tUriTestCompositon> atUris = {
            {"https://google.com",                  "https",    "google.com",       443,    {},    true},
            {"http://google.com",                   "http",     "google.com",       80,     {},    true},
            {"http://google.com:8080",              "http",     "google.com",       8080,   {},    true},
            {"http://google.com:8080/",             "http",     "google.com",       8080,   {},    true},
            {"http://google.com:8080/test",         "http",     "google.com",       8080,   {},    true},
            {"http://google.com?hallo=hallo",       "http",     "google.com",       80,
                    {
                            {"hallo", "hallo"}
                    }, true},
            {"http://google.com/?hallo=hallo",       "http",     "google.com",       80,
                    {
                            {"hallo", "hallo"}
                    }, true},
            {"http://google.com?hallo=hallo&ik=ben", "http",     "google.com",       80,
                    {
                            {"hallo", "hallo"},
                            {"ik", "ben"}
                    }, true},
            {"http://google.com/?hallo=hallo&ik=ben", "http",     "google.com",       80,
                    {
                            {"hallo", "hallo"},
                            {"ik", "ben"}
                    }, true},
            {"http://google.com:/",                 "http",     "",                 0,      {},    false},
            {"http://google.com:",                  "http",     "",                 0,      {},    false},
            {"realestate://google.com",             "",         "",                 0,      {},    false},
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
            if (tUri.asParameters.size() > 0)
                for (auto& [key, value] : tUri.asParameters)
                {
                    EXPECT_TRUE(oUri.pasParameters.find(key) != oUri.pasParameters.end());
                    EXPECT_TRUE(oUri.pasParameters.at(key) == oUri.pasParameters.at(key));
                }
        }
    }
}