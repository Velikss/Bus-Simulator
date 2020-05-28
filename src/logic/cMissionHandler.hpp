#pragma once

#include <pch.hpp>
#include <objects/cBusStop.hpp>

class cMissionHandler
{
private:
    std::deque<cBusStop*> poRoute;

public:
    cMissionHandler(cBusStop* pBusStop)
    {
        poRoute.push_back(pBusStop);
    }

    void Update();
    void AddStop(cBusStop* pBusStop);
    void UnloadBusStop(cBusStop* oBusStop);
    void UnloadMissionHandler();
    std::deque<cBusStop*>& GetRouteQueue();
};

void cMissionHandler::Update()
{
    for(auto & busStop : poRoute)
    {
        busStop->poEntityGroup->UpdateEntities();
    }
}

void cMissionHandler::AddStop(cBusStop* pBusStop)
{
    poRoute.push_back(pBusStop);
}

// Resets all the passengers currently on the busStop and removes them from the entityGroup
void cMissionHandler::UnloadBusStop(cBusStop* oBusStop)
{
    std::vector<IEntity *> *entities;
    oBusStop->poEntityGroup->GetEntityList(&entities);
    for (int i = 0; i < oBusStop->poEntityGroup->GetEntities()->size(); i++)
    {
        // TODO make default vec3 position global const
        (*entities)[i]->SetPosition(glm::vec3(200.0f, 0.15f, -200.0f));
    }
    oBusStop->poEntityGroup->ClearEntities();
}

// Unload all the busStops on the route of the missionHandler
void cMissionHandler::UnloadMissionHandler()
{
    for(uint i = 0; i < poRoute.size(); i++)
    {
        UnloadBusStop(poRoute[i]);
    }
}

std::deque<cBusStop *>& cMissionHandler::GetRouteQueue()
{
    return poRoute;
}