#include <pch.hpp>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <scenes/Scene.hpp>
#include <scenes/MyStreetScene.hpp>
#include <scenes/TestScene.hpp>

const int WIDTH = 800, HEIGHT = 600;
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

void idle(void)
{
    // Force GLUT to render the scene
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    std::cout << "hello from engine." << std::endl;

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Loading...");
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE_ARB);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // setup render.
    glutDisplayFunc(Render);
    glutTimerFunc(DELTA_TIME, Render, 0);

    glewInit();

    // first we initialize the scene.
    activeScene = new MyStreetScene(WIDTH, HEIGHT);
    // load all resources.
    activeScene->Load();

    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(SpecialKeyDown);
    glutSpecialUpFunc(SpecialKeyUp);
    glutPassiveMotionFunc(mouseMove);
    glutReshapeFunc(changeSize);
    glutIdleFunc(idle);

    // detect current anti aliasing settings
    GLint iMultiSample = 0;
    GLint iNumSamples = 0;
    glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
    glGetIntegerv(GL_SAMPLES, &iNumSamples);
    string Title = "Scene: " + activeScene->name + ", Anti-aliasing: " + (iMultiSample == 1 ? "on" : "off") + ", MSAA: " + std::to_string(iNumSamples) + "x";
    glutSetWindowTitle(Title.c_str());

    glutFullScreen();

    glutMainLoop();

    return 0;
}

