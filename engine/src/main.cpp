#include <pch.hpp>
#include <iostream>
#include <GL/glew.h>
#include <scenes/Scene.hpp>
#include <scenes/MyStreetScene.hpp>
#include <GLFW/glfw3.h>
#include <scripting/cScriptingEngine.hpp>
#include <entities/cEntity.hpp>
#include <entities/cEntityGroup.hpp>

const int WIDTH = 1080, HEIGHT = 720;

Scene *activeScene = nullptr;
GLFWwindow *window;

void keyDown(unsigned char k, int a, int b)
{
    activeScene->KeyDown(k, a, b);
}

void keyUp(unsigned char k, int a, int b)
{
    activeScene->KeyUp(k, a, b);
}

void mouseMove(int mx, int my)
{

    activeScene->MoveMouse(mx, my);
}

void changeSize(int w, int h)
{
    float ratio = w * 1.0 / h;
    glViewport(0, 0, w, h);
    activeScene->camera.aspectRatio = ratio;
}

void InitGLFW(void)
{
    if (!glfwInit())
        return;

    window = glfwCreateWindow(WIDTH, HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        glfwTerminate();
        return;
    }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    char keyChar = 0;
    switch (key)
    {
        case GLFW_KEY_W:
            keyChar = 'w';
            break;
        case GLFW_KEY_A:
            keyChar = 'a';
            break;
        case GLFW_KEY_S:
            keyChar = 's';
            break;
        case GLFW_KEY_D:
            keyChar = 'd';
            break;
        case GLFW_KEY_ESCAPE:
            keyChar = 27;
            break;
    }
    if (keyChar != 0)
    {
        if (action == GLFW_PRESS) keyDown(keyChar, 0, 0);
        if (action == GLFW_RELEASE) keyUp(keyChar, 0, 0);
    }
}

static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    static bool firstMouse = true;
    static float lastX = WIDTH, lastY = HEIGHT;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    mouseMove(xoffset, yoffset);
}

void window_size_callback(GLFWwindow *window, int width, int height)
{
    changeSize(width, height);
}

int main(int argc, char **argv)
{
    // Create static behaviours
    cBehaviourHandler::AddBehaviour("seperation", "src/scripting/seperation.js");


    cEntityGroup entityGroup;
    cBehaviourHandler *cb = new cBehaviourHandler("seperation"); // behaviourname should be a name previously defined as static behaviour

//    cEntity en2(nullptr);
//    cEntity en3(nullptr);
//    cEntity en4(nullptr);
//    cEntity en5(nullptr);

    cEntity en2(nullptr);
    en2.SetPosition(11.1f, 11.1f);
    en2.pfMaxSpeed = 10.0f;
    cEntity en3(nullptr);
    en3.SetPosition(12.1f, 12.1f);
    en3.pfMaxSpeed = 10.0f;
    cEntity en4(nullptr);
    en4.SetPosition(13.1f, 13.1f);
    en4.pfMaxSpeed = 10.0f;
    cEntity en5(nullptr);
    en5.SetPosition(14.1f, 14.1f);
    en5.pfMaxSpeed = 10.0f;

    entityGroup.AddEntity(&en2);
    entityGroup.AddEntity(&en3);
    entityGroup.AddEntity(&en4);
    entityGroup.AddEntity(&en5);

    entityGroup.AddBehaviour(cb);

    for(int i = 0; i<20; i++){
        entityGroup.UpdateEntities();

        std::cout << std::endl;
        std::cout << "Entity velocity: " << en2.pVelocity.x << " " << en2.pVelocity.y << std::endl;
        std::cout << "Entity velocity: " << en3.pVelocity.x << " " << en3.pVelocity.y << std::endl;
        std::cout << "Entity velocity: " << en4.pVelocity.x << " " << en4.pVelocity.y << std::endl;
        std::cout << "Entity velocity: " << en5.pVelocity.x << " " << en5.pVelocity.y << std::endl;
        std::cout << "Entity Steering force: " << en2.pSteeringForce.x << " - " << en2.pSteeringForce.y << std::endl;
        std::cout << "Entity Steering force: " << en3.pSteeringForce.x << " - " << en3.pSteeringForce.y << std::endl;
        std::cout << "Entity Steering force: " << en4.pSteeringForce.x << " - " << en4.pSteeringForce.y << std::endl;
        std::cout << "Entity Steering force: " << en5.pSteeringForce.x << " - " << en5.pSteeringForce.y << std::endl;
        std::cout << "Entity position: " << en2.pPosition.x << " - " << en2.pPosition.z << std::endl;
        std::cout << "Entity position: " << en3.pPosition.x << " - " << en3.pPosition.z << std::endl;
        std::cout << "Entity position: " << en4.pPosition.x << " - " << en4.pPosition.z << std::endl;
        std::cout << "Entity position: " << en5.pPosition.x << " - " << en5.pPosition.z << std::endl;
    }

}

