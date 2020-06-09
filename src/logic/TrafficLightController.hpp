#pragma once
#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>
#include <vulkan/scene/LightObject.hpp>
#include <limits>

typedef std::pair<glm::vec3, std::vector<cBaseObject*>> tTrafficLightGroup;

const uint C_MAX_TRAFFICLIGHTS_COUNT = 4;

class cTrafficLightController
{
    const glm::vec3 C_RED_LIGHT = {1.0f, 0.0f, 0.0f};
    const glm::vec3 C_GREEN_LIGHT = {42.0f, 189.0f, 88.0f};
    const float pfLightREDIntensity = 10.0f;
    const float pfLightGREENIntensity = 6.0f;

    std::vector<tTrafficLightGroup> paTrafficGroups;
    std::vector<cLightObject*> paLightsSet1;
    std::vector<cLightObject*> paLightsSet2;
    bool pbUseSet1 = true;
    uint puiLastIndex = 0;

    ushort pusSwitchTick = 5;
    ushort pusSwitchState = 0;
    ushort pusSwitchStateMax = 0;
public:
    cTrafficLightController(const string& sControllerName, std::map<string, cBaseObject*> & aObjects, cMesh* pMesh)
    {
        paLightsSet1.reserve(C_MAX_TRAFFICLIGHTS_COUNT);
        paLightsSet2.reserve(C_MAX_TRAFFICLIGHTS_COUNT);
        for (uint i = 0; i < C_MAX_TRAFFICLIGHTS_COUNT * 2; i++)
        {
            cLightObject* poLight = new cLightObject(pMesh, C_GREEN_LIGHT, pfLightREDIntensity, nullptr, false);
            poLight->SetPosition({0.0f, 5.8f, 0.0f});
            poLight->pbVisible = false;

            if (pbUseSet1)
                paLightsSet1.push_back(poLight);
            else
                paLightsSet2.push_back(poLight);
            pbUseSet1 = !pbUseSet1;

            aObjects.insert({sControllerName + "_light_" + std::to_string(i), poLight});
        }
    }

    void SetTimer(ushort uiSeconds);

    void AddGroup(const std::vector<cBaseObject*>& aGroup);

    void Update(const glm::vec3& oPosition);
};

void cTrafficLightController::Update(const glm::vec3& oPosition)
{
    if (pusSwitchStateMax == 0) SetTimer(pusSwitchTick);

    time_t t = time(NULL);
    tm* now = gmtime(&t);
    time_t uiTime = mktime(now);

    uint uiIndex = 0;
    float fDistance = std::numeric_limits<float>::max();
    for (uint i = 0; i < paTrafficGroups.size(); i++)
    {
        float fTempDistance = glm::distance(oPosition, paTrafficGroups[i].first);
        if (fTempDistance < fDistance)
        {
            uiIndex = i;
            fDistance = fTempDistance;
        }
    }

    auto& aGroup = paTrafficGroups[uiIndex].second;
    pusSwitchState = (ushort) ((uiTime / pusSwitchTick) % aGroup.size());

    if (puiLastIndex != uiIndex)
    {
        pbUseSet1 = !pbUseSet1;
        puiLastIndex = uiIndex;
    }

    auto& paLights = pbUseSet1 ? paLightsSet1 : paLightsSet2;

    for(uint i = 0; i < paLights.size(); i++)
    {
        if(i < aGroup.size())
        {
            glm::vec3 oPosition = aGroup[i]->GetPosition();
            glm::vec3 oRotation = aGroup[i]->GetRotation();

            if (oRotation.y == 0)
                paLights[i]->SetPosition(oPosition.x - 0.2f, oPosition.z - 4.8f);
            else if (oRotation.y == 180)
                paLights[i]->SetPosition(oPosition.x + 0.2f, oPosition.z + 4.8f);
            else if (oRotation.y == 90)
                paLights[i]->SetPosition(oPosition.x - 4.8f, oPosition.z - 0.2f);
            else
                paLights[i]->SetPosition(oPosition.x + 4.8f, oPosition.z + 0.2f);

            if (i == pusSwitchState)
            {
                paLights[i]->SetColor(C_GREEN_LIGHT);
                paLights[i]->SetRadius(pfLightGREENIntensity);
            }
            else
            {
                paLights[i]->SetColor(C_RED_LIGHT);
                paLights[i]->SetRadius(pfLightREDIntensity);
            }
        }
        else
            paLights[i]->SetRadius(0.0f);
    }
}

void cTrafficLightController::AddGroup(const std::vector<cBaseObject*>& aGroup)
{
    glm::vec3 oCenter = {};
    uint i = 0;
    for(; i < aGroup.size(); i++)
    {
        oCenter.x += aGroup[i]->GetPosition().x;
        oCenter.y += aGroup[i]->GetPosition().y;
        oCenter.z += aGroup[i]->GetPosition().z;
    }
    oCenter.x /= (i + 1.0f);
    oCenter.y /= (i + 1.0f);
    oCenter.z /= (i + 1.0f);
    paTrafficGroups.emplace_back(oCenter,aGroup);
}

void cTrafficLightController::SetTimer(ushort uiSeconds)
{
    if(uiSeconds == 0) uiSeconds = 1;
    pusSwitchTick = uiSeconds;
}
