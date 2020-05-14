#pragma once

#include <pch.hpp>
#include <vulkan/scene/BaseObject.hpp>

/*
The camera class, it is however FP based and should have its own sub-class in a later version.
*/
class Camera
{
public:
    float fFoV = 45.0;
    float fZNear = 1.0;
    float fZFar = 500.0;

    glm::vec3 cameraPos = glm::vec3(-8.0f, 7.0f, -3.0f);
    glm::vec3 cameraFront = glm::vec3(1.0f, 0.0f, 1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 direction;

    float yaw = 45;
    float pitch = -10;

    const float cameraSpeed = 0.1f;
    const float cameraSpeedLook = 1.0f;
    float mouseSpeed = 0.07f;

    glm::mat4 view = glm::mat4(1.0f);
public:
    float cameraHeight = 3.75f;

    bool lockHeight = true;
    bool lockMovement = false;

    // bus camera
    const float cameraScrollSpeed = 0.5f;
    float orbitDistance = 30.0f;
    cBaseObject* cameraPivotObject;
    glm::vec3 cameraPivotPos;
    glm::vec3 cameraPivotChanges;

    virtual void Forward() = 0;
    virtual void BackWard() = 0;
    virtual void MoveLeft() = 0;
    virtual void MoveRight() = 0;
    virtual void MoveUp() = 0;
    virtual void MoveDown() = 0;
    virtual void MovePivotX(float fMultiplier) = 0;
    virtual void MovePivotY(float fMultiplier) = 0;
    virtual void LookUp() = 0;
    virtual void LookDown() = 0;
    virtual void LookLeft() = 0;
    virtual void LookRight() = 0;
    // is the end op the passthrough from the mouse input.
    virtual void LookMouseDiff(int x, int y) = 0;
    // process the commits to the pv.
    virtual void LookMouseWheelDiff(float x, float y) = 0;
    virtual void ProcessUpdates() = 0;
    // sets the pv and position on the passed shader.
    virtual glm::mat4& GetViewMatrix() = 0;
    virtual glm::vec3 GetPosition() = 0;
    virtual void SetPosition(glm::vec3& position) = 0;
    virtual glm::vec3 GetFront() = 0;
    virtual void SetFront(glm::vec3& front) = 0;
    virtual float GetPitch() = 0;
    virtual void SetPitch(float pitch) = 0;
    virtual float GetYaw() = 0;
    virtual void SetYaw(float yaw) = 0;
};

class FirstPersonFlyCamera : public Camera
{
public:
    void Forward()
    {
        if (!lockMovement)
            cameraPos += cameraSpeed * cameraFront;
    }

    void BackWard()
    {
        if (!lockMovement)
            cameraPos -= cameraSpeed * cameraFront;
    }

    void MoveLeft()
    {
        if (!lockMovement)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    void MoveRight()
    {
        if (!lockMovement)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    void MoveUp()
    {
        if (!lockMovement)
            cameraHeight += cameraSpeed;
//            cameraPos += cameraSpeed * cameraUp;
    }

    void MoveDown()
    {
        if (!lockMovement)
            cameraHeight -= cameraSpeed;
//            cameraPos -= cameraSpeed * cameraUp;
    }

    void MovePivotX(float fMultiplier)
    {

    }

    void MovePivotY(float fMultiplier)
    {

    }

    void LookUp()
    {
        pitch += cameraSpeedLook;
    }

    void LookDown()
    {
        pitch -= cameraSpeedLook;
    }

    void LookLeft()
    {
        yaw -= cameraSpeedLook;
    }

    void LookRight()
    {
        yaw += cameraSpeedLook;
    }

    // is the end op the passthrough from the mouse input.
    void LookMouseDiff(int x, int y)
    {
        yaw += mouseSpeed * x;
        pitch -= mouseSpeed * y;
        // no flipping to the other side
        if (pitch < -80)
            pitch = -80;
        if (pitch > 80)
            pitch = 80;
    }

    void LookMouseWheelDiff(float x, float y)
    {

    }

    // process the commits to the pv.
    void ProcessUpdates()
    {
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);

        cameraPos.y = cameraHeight;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }

    // sets the pv and position on the passed shader.
    /*void SetTransformationOnShader(ShaderProgram* shader)
    {
        shader->Bind();
        //shader->setMat4("pv", this->pv);
        shader->setVec3("viewPos", this->GetPosition());
    }*/

    glm::mat4& GetViewMatrix()
    {
        return this->view;
    }

    glm::vec3 GetPosition()
    {
        return this->cameraPos;
    }

    void SetPosition(glm::vec3& position)
    {
        this->cameraPos = position;
    }

    glm::vec3 GetFront()
    {
        return this->cameraFront;
    }

    void SetFront(glm::vec3& front)
    {
        this->cameraFront = front;
    }

    float GetPitch()
    {
        return this->pitch;
    }

    void SetPitch(float pitch)
    {
        this->pitch = pitch;
    }

    float GetYaw()
    {
        return this->yaw;
    }

    void SetYaw(float yaw)
    {
        this->yaw = yaw;
    }
};