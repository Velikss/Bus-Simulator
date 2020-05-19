#pragma once
#include <pch.hpp>
#include <gtest/gtest.h>
#include <vulkan/entities/cBehaviourHandler.hpp>
#include <vulkan/entities/cEntityGroup.hpp>

TEST(ScriptingTests, CheckIfCoordinatesChangeAfterSeperationUpdate)
{
    cBehaviourHandler::AddBehaviour("seperation", "../engine/src/scripting/seperation.js");

    cEntityGroup entityGroup;
    cBehaviourHandler *cb = new cBehaviourHandler("seperation");

    cEntity en2(nullptr);
    en2.SetPosition(glm::vec3(11.1f, 0, 17.1f));
    en2.pfMaxSpeed = 10.0f;
    cEntity en3(nullptr);
    en3.SetPosition(glm::vec3(12.1f, 0, 177.1f));
    en3.pfMaxSpeed = 10.0f;
    cEntity en4(nullptr);
    en4.SetPosition(glm::vec3(13.1f, 0, 22.1f));
    en4.pfMaxSpeed = 10.0f;
    cEntity en5(nullptr);
    en5.SetPosition(glm::vec3(14.1f, 0, 1.1f));
    en5.pfMaxSpeed = 10.0f;

    entityGroup.AddEntity(&en2);
    entityGroup.AddEntity(&en3);
    entityGroup.AddEntity(&en4);
    entityGroup.AddEntity(&en5);

    entityGroup.AddBehaviour(cb);

    glm::vec3 old = en2.poPosition;

    glm::vec3 oldDiff = en2.poPosition - en3.poPosition;

    entityGroup.UpdateEntities();

    glm::vec3 newDiff = en2.poPosition - en3.poPosition;

    ASSERT_NE(old[0], en2.poPosition[0]);
    ASSERT_NE(old[2], en2.poPosition[2]);
    ASSERT_NE(oldDiff, newDiff);
}

TEST(ScriptingTests, CheckIfCoordinatesChangeAfterCohesionUpdate)
{
    cBehaviourHandler::AddBehaviour("cohesion", "../engine/src/scripting/cohesion.js");

    cEntityGroup entityGroup;
    cBehaviourHandler *cb = new cBehaviourHandler("cohesion");

    cEntity en2(nullptr);
    en2.SetPosition(glm::vec3(11.1f, 0, 17.1f));
    en2.pfMaxSpeed = 10.0f;
    cEntity en3(nullptr);
    en3.SetPosition(glm::vec3(12.1f, 0, 177.1f));
    en3.pfMaxSpeed = 10.0f;
    cEntity en4(nullptr);
    en4.SetPosition(glm::vec3(13.1f, 0, 22.1f));
    en4.pfMaxSpeed = 10.0f;
    cEntity en5(nullptr);
    en5.SetPosition(glm::vec3(14.1f, 0, 1.1f));
    en5.pfMaxSpeed = 10.0f;

    entityGroup.AddEntity(&en2);
    entityGroup.AddEntity(&en3);
    entityGroup.AddEntity(&en4);
    entityGroup.AddEntity(&en5);

    entityGroup.AddBehaviour(cb);

    glm::vec3 old = en2.poPosition;

    glm::vec3 oldDiff = en2.poPosition - en3.poPosition;

    entityGroup.UpdateEntities();

    glm::vec3 newDiff = en2.poPosition - en3.poPosition;

    ASSERT_NE(old[0], en2.poPosition[0]);
    ASSERT_NE(old[2], en2.poPosition[2]);
    ASSERT_NE(oldDiff, newDiff);
}