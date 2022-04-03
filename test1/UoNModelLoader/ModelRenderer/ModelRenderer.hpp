/**
 * @file ModelRenderer.hpp
 * @author Christodoulos Sotiriou
 * @brief Header File: Renders a 3D model in a new windows
 * @version 0.1
 * @date 2021-11-16
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef MODEL_RENDERER_HPP
#define MODEL_RENDERER_HPP

#include <string>

#include "GL/freeglut.h"
#include "GL/gl.h"

#include "Model.hpp"

#if (defined(WIN32) | defined(_WIN32)) && defined(BUILD_SHARED_LIBS)
#if defined(UoNModelLoader_EXPORTS)
#define MODELRENDERERLIB_API __declspec(dllexport)
#else
#define MODELRENDERERLIB_API __declspec(dllimport)
#endif
#else
#define MODELRENDERERLIB_API
#endif

/**
 * Using a singleton design for the renderer
 * Source:
 * https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
 */

class ModelRenderer {
  public:
    static ModelRenderer &getInstance() {
        static ModelRenderer instance; // Guaranteed to be destroyed.
                                       // Instantiated on first use.
        return instance;
    }

  private:
    ModelRenderer() {} // Constructor? (the {} brackets) are needed here.

    // C++ 11
    // =======
    // We can use the better technique of deleting the methods
    // we don't want.
  public:
    ModelRenderer(ModelRenderer const &) = delete;
    void operator=(ModelRenderer const &) = delete;

    // Note: Scott Meyers mentions in his Effective Modern
    //       C++ book, that deleted functions should generally
    //       be public as it results in better error messages
    //       due to the compilers behavior to check accessibility
    //       before deleted status

  public:
    MODELRENDERERLIB_API int Initialize(int *argc, char **argv);
    MODELRENDERERLIB_API int Initialize(int *argc, char **argv,
                                        std::string windowName);

    MODELRENDERERLIB_API void renderModel(Model *modelToRender);
    MODELRENDERERLIB_API void removeCurrentModel();

  private:
    static Model *model;
    static float modelAngle;
    static float modelScale;

    static float cameraDirectionX, cameraDirectionZ;
    static float cameraPositionX, cameraPositionZ;

    static int xOrigin;
    static float deltaAngle;
    static float cameraAngle;

    static void drawScreen();
    static void keyboardEventCallback(unsigned char key, int x, int y);
    static void modelRotate(int value);
    static void scrollWheelCallback(int button, int dir, int x, int y);
    static void mouseButton(int button, int state, int x, int y);
    static void mouseMove(int x, int y);
    static void windowResizeCallback(int width, int height);
};

#endif
