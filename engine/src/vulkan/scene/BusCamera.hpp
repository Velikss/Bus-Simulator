#pragma once

#include <pch.hpp>
#include "scenes/Camera.hpp"

class BusCamera : public Camera
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
            cameraPos += cameraSpeed * cameraUp;
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
        if(y < 0 && orbitDistance < 55.0f)
        {
            orbitDistance += cameraScrollSpeed;
        }
        if(y > 0 && orbitDistance > 20.0f)
        {
            orbitDistance -= cameraScrollSpeed;
        }
    }

    // process the commits to the pv.
    void ProcessUpdates()
    {
        cameraPos.x = sin(glm::radians(yaw)) * orbitDistance + cameraPivot->x;
        cameraPos.y = cameraPivot->y + cameraHeight;
        cameraPos.z = cos(glm::radians(yaw)) * orbitDistance + cameraPivot->z;


        cameraFront = glm::normalize(*cameraPivot - cameraPos);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraFront));
        cameraUp = glm::normalize(glm::cross(cameraFront, cameraRight));

//        view = glm::lookAt(cameraPos, glm::vec3(cameraPivot->x + cameraPivotChanges.x,
//                                                cameraPivot->y + cameraPivotChanges.y,
//                                                cameraPivot->z + cameraPivotChanges.z) + cameraFront, cameraUp);
        view = glm::lookAt(cameraPos, *cameraPivot + cameraPivotChanges + cameraFront, cameraUp);

        proj = glm::perspective(
                FoV,
                aspectRatio,
                ZNear,
                zFar);

        pv = proj * view;
    }

    // sets the pv and position on the passed shader.
    void SetTransformationOnShader(ShaderProgram* shader)
    {
        shader->Bind();
        shader->setMat4("pv", this->pv);
        shader->setVec3("viewPos", this->GetPosition());
    }

    glm::mat4& GetProjectionView()
    {
        return this->pv;
    }

    glm::mat4& GetViewMatrix()
    {
        return this->view;
    }

    glm::mat4& GetProjectionMatrix()
    {
        return this->proj;
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



