#pragma once
#include <pch.hpp>
#include <gtest/gtest.h>
#include <engine/src/entities/cBehaviourHandler.hpp>
#include <engine/src/entities/cEntityGroup.hpp>

TEST(ScriptingTests, CheckIfCoordinatesChangeAfterSeperationUpdate)
{
    cBehaviourHandler::AddBehaviour("seperation", "src/scripting/seperation.js");

    cEntityGroup entityGroup;
    cBehaviourHandler *cb = new cBehaviourHandler("seperation");

    cEntity en2(nullptr);
    en2.SetPosition(11.1f, 17.1f);
    en2.pfMaxSpeed = 10.0f;
    cEntity en3(nullptr);
    en3.SetPosition(12.1f, 177.1f);
    en3.pfMaxSpeed = 10.0f;
    cEntity en4(nullptr);
    en4.SetPosition(13.1f, 22.1f);
    en4.pfMaxSpeed = 10.0f;
    cEntity en5(nullptr);
    en5.SetPosition(14.1f, 1.1f);
    en5.pfMaxSpeed = 10.0f;

    entityGroup.AddEntity(&en2);
    entityGroup.AddEntity(&en3);
    entityGroup.AddEntity(&en4);
    entityGroup.AddEntity(&en5);

    entityGroup.AddBehaviour(cb);

    glm::vec3 old = en2.pPosition;

        entityGroup.UpdateEntities();

    ASSERT_NE(old[0], en2.pPosition[0]);

    ASSERT_NE(old[2], en2.pPosition[2]);
}