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
    bool bValid;
};

TEST(UriTests, ParseFromString)
{
    std::vector<tUriTestCompositon> atUris = {
            {"https://google.com",          "https",    "google.com",    443,    true},
            {"http://google.com",           "http",     "google.com",    80,     true},
            {"realestate://google.com",     "",         "",              0,      false},
            {"http://google.com:8080",      "http",     "google.com",    8080,   true},
            {"http://google.com:8080/",     "http",     "google.com",    8080,   true},
            {"http://google.com:8080/test", "http",     "google.com",    8080,   true},
    };
    for (auto& tUri : atUris)
    {
        auto oUri = cUri::ParseFromString(tUri.sUri);
        EXPECT_EQ(oUri.psProtocol, tUri.sProtocol);
        EXPECT_EQ(oUri.psHost, tUri.sHost);
        EXPECT_EQ(oUri.pusPort, tUri.usPort);
        EXPECT_EQ(oUri.IsValidUri(), tUri.bValid);
    }
}