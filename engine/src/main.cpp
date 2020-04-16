#include <pch.hpp>
#include <iostream>
#include <GL/glew.h>
#include <scenes/Scene.hpp>
#include <scenes/MyStreetScene.hpp>
#include <scenes/TestScene.hpp>
#include <GLFW/glfw3.h>

const int WIDTH = 1080, HEIGHT = 720;
unsigned const int DELTA_TIME = 1;

Scene* activeScene = nullptr;

void Render()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    activeScene->Update();
    activeScene->Render();

    glutSwapBuffers();
}
void Render(int n)
{
    Render();
    glutTimerFunc(DELTA_TIME, Render, 0);
}

void keyDown(unsigned char k, int a, int b)
{
    activeScene->KeyDown(k, a, b);
}
void keyUp(unsigned char k, int a, int b)
{
    activeScene->KeyUp(k, a, b);
}
void SpecialKeyDown(int key, int x, int y)
{
    activeScene->KeyDown((byte)(128 + key), x, y);
}
void SpecialKeyUp(int key, int x, int y)
{
    activeScene->KeyUp((byte)(128 + key), x, y);
}
void mouseMove(int mx, int my) {

    activeScene->MoveMouse(mx, my);
}

void changeSize(int w, int h)
{
    float ratio = w * 1.0 / h;
    glViewport(0, 0, w, h);
    activeScene->camera.aspectRatio = ratio;
}

GLFWwindow* window;

void InitGLFW(void)
{
    if (! glfwInit())
        return;

    window = glfwCreateWindow(WIDTH, HEIGHT, "Hello World", NULL, NULL);
    if (! window)
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    char keyChar = 0;
    switch (key) {
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
    if (keyChar != 0) {
        if (action == GLFW_PRESS) keyDown(keyChar, 0, 0);
        if (action == GLFW_RELEASE) keyUp(keyChar, 0, 0);
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    static bool firstMouse = true;
    static float lastX = WIDTH, lastY = HEIGHT;

    if(firstMouse)
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

void window_size_callback(GLFWwindow* window, int width, int height)
{
    changeSize(width, height);
}

int main(int argc, char** argv)
{
    std::cout << "hello from engine." << std::endl;

    InitGLFW();

    /*glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Loading...");*/
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE_ARB);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // setup render.
    //glutDisplayFunc(Render);
    //glutTimerFunc(DELTA_TIME, Render, 0);

    //glewInit();

    // first we initialize the scene.
    activeScene = new MyStreetScene(WIDTH, HEIGHT);
    // load all resources.
    activeScene->Load();

    /*glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(SpecialKeyDown);
    glutSpecialUpFunc(SpecialKeyUp);
    glutPassiveMotionFunc(mouseMove);
    glutReshapeFunc(changeSize);*/

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

    // detect current anti aliasing settings
    GLint iMultiSample = 0;
    GLint iNumSamples = 0;
    glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
    glGetIntegerv(GL_SAMPLES, &iNumSamples);
    string Title = "Scene: " + activeScene->name + ", Anti-aliasing: " + (iMultiSample == 1 ? "on" : "off") + ", MSAA: " + std::to_string(iNumSamples) + "x";
    //glutSetWindowTitle(Title.c_str());

    //glutFullScreen();

    //glutMainLoop();

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        activeScene->Update();
        activeScene->Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

