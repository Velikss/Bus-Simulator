#include <pch.hpp>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

void Render()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glutSwapBuffers();
}
void Render(int n)
{
    Render();
    glutTimerFunc(1, Render, 0);
}

int main(int argc, char** argv)
{
    std::cout << "hello from engine." << std::endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Hello OpenGL");

    // setup render.
    glutDisplayFunc(Render);
    glutTimerFunc(1, Render, 0);

    glewInit();

    glutMainLoop();

    return 0;
}

