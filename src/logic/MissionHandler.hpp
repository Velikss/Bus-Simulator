#pragma once

#include <pch.hpp>
#include <objects/BusStop.hpp>
#include <entities/Passenger.hpp>

const glm::vec3 C_DEFAULT_PASSENGER_LOCATION = glm::vec3(200.0f, 0.15f, -200.0f);
const float C_RADIUS = 4;

class cMissionHandler
{
private:
    std::vector<cBusStop*> poRoute;
    std::deque<cBusStop*> poActiveRoute;

public:
    void Update();
    void AddStop(cBusStop* pBusStop);
    void UnloadBusStop(cBusStop* oBusStop);
    void SetPassengersDestinations();
    void UnloadRouteBusStops();
    void RefillActiveRouteQueue();
    cBusStop* BusStopWithinRadius(glm::vec3 oBusDoorPos);
    std::deque<cBusStop*>& GetRouteQueue();
    bool PassengersAvailable(cBusStop* oBusStop);
};

void cMissionHandler::Update()
{
    // TODO only update when needed to move
    for(auto & busStop : poRoute)
    {
        busStop->poEntityGroup->UpdateEntities();
    }
}

// Resets all the passengers currently on the busStop and removes them from the entityGroup
void cMissionHandler::UnloadBusStop(cBusStop* oBusStop)
{
    std::vector<IEntity*>* entities;
    oBusStop->poEntityGroup->GetEntityList(&entities);
    for (int i = 0; i < oBusStop->poEntityGroup->GetEntities()->size(); i++)
    {
        (*entities)[i]->SetPosition(C_DEFAULT_PASSENGER_LOCATION);
        (*entities)[i]->pbVisible = false;
    }
    oBusStop->poEntityGroup->ClearEntities();
}

// check if bus stop has available passengers
// by checking if all passengers at the bus stop have this bus stop as their destination
bool cMissionHandler::PassengersAvailable(cBusStop* oBusStop)
{
    std::vector<IEntity*>* entities;
    oBusStop->poEntityGroup->GetEntityList(&entities);

    for (uint i = 0; i < oBusStop->poEntityGroup->GetEntities()->size(); i++)
    {
        cPassenger* oPassenger = dynamic_cast<cPassenger*>((*entities)[i]);

        if(!oPassenger->DestinationEqual(oBusStop))
        {
            return true;
        }
    }
    return false;
}

// Set random destinations of passengers
void cMissionHandler::SetPassengersDestinations()
{
    // TODO currently all passengers on a bus stop get the same destination
    for(uint i = 0; i < poActiveRoute.size(); i++)
    {
        // last bus stop does not receive any passengers
        if(i == poActiveRoute.size() - 1)
            break;
        // Create random destination number.
        // This number corresponds with the index of a bus stop that is ahead of the current bus stop.
        int iRandDestIndex = rand() % (poActiveRoute.size() - (i + 1)) + (i + 1);

        // set destination for all passengers
        std::vector<IEntity*>* entities;
        poActiveRoute[i]->poEntityGroup->GetEntityList(&entities);
        for (uint j = 0; j < poActiveRoute[i]->poEntityGroup->GetEntities()->size(); j++)
        {
            dynamic_cast<cPassenger*>((*entities)[j])->SetDestination(poActiveRoute[iRandDestIndex]);
        }
    }
}

// Unload all the busStops on the route of the missionHandler
void cMissionHandler::UnloadRouteBusStops()
{
    for(uint i = 0; i < poRoute.size(); i++)
    {
        UnloadBusStop(poRoute[i]);
    }
}

void cMissionHandler::RefillActiveRouteQueue()
{
    poActiveRoute.clear();
    for(uint i = 0; i < poRoute.size(); i++)
        poActiveRoute.push_back(poRoute[i]);
}

// return the bus stop that is within a certain radius of the bus door
cBusStop* cMissionHandler::BusStopWithinRadius(glm::vec3 oBusDoorPos)
{
    for(uint i = 0; i < poActiveRoute.size(); i++)
    {
        glm::vec3 oBusStopPos = poActiveRoute[i]->GetPosition();
        // squared distance between the bus door and the bus stop
        float distSquared = ((oBusDoorPos.x - oBusStopPos.x) * (oBusDoorPos.x - oBusStopPos.x)) +
                            ((oBusDoorPos.z - oBusStopPos.z) * (oBusDoorPos.z - oBusStopPos.z));

        // Check if bus door within certain radius of the bus stop
        if (distSquared < C_RADIUS * C_RADIUS && distSquared > 0)
            return poActiveRoute[i];
    }
    return nullptr;
}

void cMissionHandler::AddStop(cBusStop* pBusStop)
{
    poRoute.push_back(pBusStop);
    poActiveRoute.push_back(pBusStop);
}

std::deque<cBusStop*>& cMissionHandler::GetRouteQueue()
{
    return poActiveRoute;
}