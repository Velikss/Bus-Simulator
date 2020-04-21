#include <pch.hpp>
#include <iostream>
#include <GL/glew.h>
#include <scenes/Scene.hpp>
#include <scenes/MyStreetScene.hpp>
#include <GLFW/glfw3.h>
#include <scripting/cScriptingEngine.hpp>

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
    cScriptingEngine engine = cScriptingEngine();

    cEntity en = cEntity(glm::vec3(5.0f, 2.0f, 3.0f));
    std::vector<cEntity *> entities = std::vector<cEntity *>();

    engine.ExecuteBehaviour("src/scripting/seperation.js", &en, entities);
}

