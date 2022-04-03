/**
 * @file ModelRenderer.cpp
 * @author Christodoulos Sotiriou
 * @brief Source File: Renders a 3D model in a new windows
 * @version 0.1
 * @date 2021-11-16
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "ModelRenderer.hpp"
#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <math.h>
#include <vector>

static float modelAngleChange = 0;

float ModelRenderer::modelAngle = 0;
float ModelRenderer::modelScale = 1;
Model *ModelRenderer::model = NULL;

float ModelRenderer::cameraDirectionX = 0.0;
float ModelRenderer::cameraDirectionZ = -1.0;
float ModelRenderer::cameraPositionX = 0.0;
float ModelRenderer::cameraPositionZ = 5.0;

float ModelRenderer::cameraAngle = 0.0f;
float ModelRenderer::deltaAngle = 0.0f;
int ModelRenderer::xOrigin = -1;

int ModelRenderer::Initialize(int *argc, char **argv) {
    return Initialize(argc, argv, "3D Model Renderer");
}

int ModelRenderer::Initialize(int *argc, char **argv, std::string windowName) {
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(windowName.c_str());

    /* Setup Callbacks */
    glutDisplayFunc(drawScreen);
    glutKeyboardFunc(keyboardEventCallback);
    glutTimerFunc(25, modelRotate, 0);
    glutMouseWheelFunc(scrollWheelCallback);
    glutReshapeFunc(windowResizeCallback);

    /* Mouse Control */
    // https://www.lighthouse3d.com/tutorials/glut-tutorial/mouse-putting-it-all-together/
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMove);

    /* Enable depth buffer */
    glEnable(GL_DEPTH_TEST);

    /* Setup Lighing */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    float lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float lightAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float lightPosition[] = {0.0f, 0.0f, -1.0f, 1.0f};

    // Ambient light component
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    // Diffuse light component
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    // Light position
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glutMainLoop();

    return 0;
}

void ModelRenderer::renderModel(Model *modelToRender) { model = modelToRender; }

void ModelRenderer::removeCurrentModel() { model = NULL; }

void drawVerticies(std::vector<Vector> verticies, int vertexOrder[],
                   int vertexOrderSize) {
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i < vertexOrderSize; i++) {
        glVertex3f(verticies[vertexOrder[i]].x, verticies[vertexOrder[i]].y,
                   verticies[vertexOrder[i]].z);
    }
    glEnd();
}

void ModelRenderer::drawScreen() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    // Reset transformations
    glLoadIdentity();
    // Set the camera
    gluLookAt(cameraPositionX, 1.25f, cameraPositionZ,
              cameraPositionX + cameraDirectionX, 1.0f,
              cameraPositionZ + cameraDirectionZ, 0.0f, 1.0f, 0.0f);

    /* Ratation around each axis */
    glRotatef(modelAngle, 3.0f, 1.0f,
              2.0f); // rotating object continuously by 2 degree
    /* Handle Zooming */
    glScalef(modelScale, modelScale, modelScale);

    if (model != NULL) {
        for (auto const &it : model->cells) {
            Cell &cell = it.second;
            std::vector<Vector> verticies = cell.getVertices();

            if (cell.VERTEX_COUNT() == 4) {
                int vertexOrder[] = {0, 1, 2, 3, 0, 1};
                drawVerticies(verticies, vertexOrder, 6);
            } else if (cell.VERTEX_COUNT() == 5) {
                int vertexOrder[] = {0, 1, 2, 3, 4, 0, 1};
                drawVerticies(verticies, vertexOrder, 7);
            } else if (cell.VERTEX_COUNT() == 8) {
                // http://www.cs.umd.edu/gvil/papers/av_ts.pdf
                int vertexOrder[] = {3, 2, 0, 1, 5, 2, 6, 3, 7, 0, 4, 5, 7, 6};
                drawVerticies(verticies, vertexOrder,
                              sizeof(vertexOrder) / sizeof(int));
            }
        }
    }

    glPopMatrix();
    glutSwapBuffers();
}

void ModelRenderer::keyboardEventCallback(unsigned char key, int x, int y) {
    (void)x;
    (void)y;

    if (key == 'q') { /* Escape Key */
        exit(0);
    } else if (key == 'Q') {
        exit(1);
    } else if (key == 'w') {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glutPostRedisplay();
    } else if (key == 's') {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glutPostRedisplay();
    } else if (key == 'r') {
        if (modelAngleChange != 0)
            modelAngleChange = 0;
        else
            modelAngleChange = 2.0;
    }
}

void ModelRenderer::modelRotate(int value) {
    modelAngle += modelAngleChange;
    if (modelAngle > 360)
        modelAngle -= 360;

    glutPostRedisplay();
    glutTimerFunc(25, modelRotate, 0); /* reset timer */
}

void ModelRenderer::mouseMove(int x, int y) {
    // this will only be true when the left button is down
    if (xOrigin >= 0) {
        // update deltaAngle
        deltaAngle = (x - xOrigin) * 0.001f;

        // update camera's direction
        cameraDirectionX = sin(cameraAngle - deltaAngle);
        cameraDirectionZ = -cos(cameraAngle - deltaAngle);
    }
}

void ModelRenderer::mouseButton(int button, int state, int x, int y) {
    // only start motion if the left button is pressed
    if (button == GLUT_LEFT_BUTTON) {
        // when the button is released
        if (state == GLUT_UP) {
            cameraAngle += deltaAngle;
            xOrigin = -1;
        } else { // state = GLUT_DOWN
            xOrigin = x;
        }
    }
}

void ModelRenderer::scrollWheelCallback(int button, int dir, int x, int y) {
    if (dir == GLUT_UP) {
        modelScale *= 1.05;
        glutPostRedisplay();
    } else {
        modelScale /= 1.05;
        glutPostRedisplay();
    }
}

void ModelRenderer::windowResizeCallback(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = (float)height / (float)width;
    glOrtho(-1.0, 1.0, ((float)height / (float)width),
            -((float)height / (float)width), -10.0f, 10.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
