#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include "helper/torus.h"
#include "helper/teapot.h"
#include <glm/glm.hpp>
#include "helper/texture.h"
#include "helper/plane.h"
#include "helper/frustum.h"

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog, solidProg;
    GLuint shadowFBO, pass1Index, pass2Index;
    
    //variables for object in the scene
    Torus torus;
    Teapot teapot;
    Plane plane;

    int shadowMapWidth, shadowMapHeight;
    float tprev;

    glm::mat4 lightPV, shadowBias;

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
};

#endif // SCENEBASIC_UNIFORM_H
