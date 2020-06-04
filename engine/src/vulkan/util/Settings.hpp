#pragma once

#include <pch.hpp>

struct tResolution
{
    uint puiWidth;
    uint puiHeight;
};

class cSettings
{
public:
    static std::map<string, tResolution> pmtResolutions;

    static void Init();
};

std::map<string, tResolution> cSettings::pmtResolutions;

void cSettings::Init()
{
    pmtResolutions["3840x2160"] = {3840, 2160};
    pmtResolutions["2560x1440"] = {2560, 1440};
    pmtResolutions["2048x1536"] = {2048, 1536};
    pmtResolutions["1920x1440"] = {1920, 1440};
    pmtResolutions["1920x1080"] = {1920, 1080};
    pmtResolutions["1600x900"] = {1600, 900};
}
