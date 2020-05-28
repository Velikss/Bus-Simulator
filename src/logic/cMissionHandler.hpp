#pragma once

#include <pch.hpp>
#include <objects/cBusStop.hpp>
#include <entities/cPassenger.hpp>

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
    void SetPassengersDestinations();
    void UnloadMissionHandler();
    std::deque<cBusStop*>& GetRouteQueue();
};

void cMissionHandler::Update()
{
    // TODO only update when needed to move
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
        (*entities)[i]->pbVisible = false;
    }
    oBusStop->poEntityGroup->ClearEntities();
}

// Set random destinations of passengers
void cMissionHandler::SetPassengersDestinations()
{
    // TODO currently all passengers on a bus stop get the same destination
    for(uint i = 0; i < poRoute.size(); i++)
    {
        if(i == poRoute.size() - 1)
            break;
        // Create random destination number.
        // This number corresponds with the index of a bus stop that is ahead of the current bus stop.
        int iRandDestIndex = rand() % (poRoute.size() - (i + 1))+ (i + 1);

        // set destination for all passengers
        std::vector<IEntity *> *entities;
        poRoute[i]->poEntityGroup->GetEntityList(&entities);
        for (uint j = 0; j < poRoute[i]->poEntityGroup->GetEntities()->size(); j++)
        {
            dynamic_cast<cPassenger*>((*entities)[j])->SetDestination(poRoute[iRandDestIndex]);
        }
    }
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