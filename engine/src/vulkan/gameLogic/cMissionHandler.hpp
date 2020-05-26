#pragma once

#include <pch.hpp>
#include <vulkan/entities/cBusStop.hpp>

class cMissionHandler
{
private:
    std::queue<cBusStop*> poRoute;

public:
    cMissionHandler(cBusStop* pBusStop)
    {
        poRoute.push(pBusStop);
    }

    void AddStop(cBusStop* pBusStop);
};

void cMissionHandler::AddStop(cBusStop* pBusStop)
{
    poRoute.push(pBusStop);
}