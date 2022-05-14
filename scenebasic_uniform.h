#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include <GLFW/glfw3.h>

#include "helper/torus.h"
#include "helper/teapot.h"
#include <glm/glm.hpp>
#include "helper/texture.h"
#include "helper/plane.h"
#include "helper/frustum.h"

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog, solidProg, wireProg;
    GLuint shadowFBO, pass1Index, pass2Index;
    
    //variables for object in the scene
    Torus torus;
    Teapot teapot;
    Plane plane;

    int shadowMapWidth, shadowMapHeight, shaderID;
    float tPrev;

    glm::mat4 lightPV, shadowBias, viewport;

    float angle;

    Frustum lightFrustum;

    void setMatrices();

    void compile();

    void setupFBO();

    void drawScene();

    void spitOutDepthBuffer();

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
    void changeShader(int key);
    void key_callback(GLFWwindow* window, int key, int scancode ,int action, int mods);
};

#endif // SCENEBASIC_UNIFORM_H
