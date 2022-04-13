#include "scenebasic_uniform.h"

#include <iostream>
using std::cerr;
using std::endl;

#include <glm/gtc/matrix_transform.hpp>
using glm::vec3;
using glm::mat4;
using glm::vec4;

//variables used in update method
float angle;
float timer;
int shaderID;

//constructor for scene objects
SceneBasic_Uniform::SceneBasic_Uniform() : tPrev(0), shadowMapWidth(512), shadowMapHeight(512), teapot(14, mat4(1.0f)), plane(40.0f, 40.0f, 2.0f, 2.0f), torus(0.7f * 2.0f, 0.3f * 2.0f, 50, 50) {}

void SceneBasic_Uniform::initScene()
{
    compile();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	glEnable(GL_DEPTH_TEST);

    //variables used in update method
    angle = glm::quarter_pi<float>();
    tPrev = 0.0f;
    timer = 10.0f;

    //set up object for framebuffer
    setupFBO();

    GLuint programHandle = prog.getHandle();
    pass1Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "recordDepth");
    pass2Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "shadeWithShadow");

    shadowBias = mat4(vec4(0.5f, 0.0f, 0.0f, 0.0f),
        vec4(0.0f, 0.5f, 0.0f, 0.0f),
        vec4(0.0f, 0.0f, 0.5f, 0.0f),
        vec4(0.0f, 0.0f, 0.0f, 0.5f));

    float c = 1.65f;

    vec3 lightPos = vec3(0.0f, c * 5.25f, c * 7.5f); //in world coords

    lightFrustum.orient(lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
    lightFrustum.setPerspective(50.0f, 1.0f, 1.0f, 25.0f);
    lightPV = shadowBias * lightFrustum.getProjectionMatrix() * lightFrustum.getViewMatrix();

    prog.setUniform("Light.La", vec3(0.85f));
    prog.setUniform("Light.Ld", vec3(0.85f));
    prog.setUniform("Light.Ls", vec3(0.85f));
    prog.setUniform("ShadowMap", 0);


    //initialize shader ID to phong shading
    shaderID = 4;
    prog.setUniform("shaderID", shaderID);

    //set up textures
    GLuint metal = Texture::loadTexture("./media/texture/me_textile.png");
    GLuint moss = Texture::loadTexture("./media/texture/moss.png");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, metal);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, moss);
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update( float t )
{
	//update angle and timer, timer used for changing shader type, angle to rotate objects

    float deltaT = t - tPrev;

    if (tPrev == 0.0f) {
        deltaT = 0.0f;
    }

    tPrev = t;

    timer -= deltaT;
    angle += 4.0f * deltaT;

    //reset angle if a full rotation has been done
    if (angle > 360.0f) {
        angle = 0.0f;
    }

    //when the timer hits 0, change the shader ID given to shader
    if (timer <= 0.0f) {
        timer = 10.0f;
        
        if (!(shaderID == 3)) {
            shaderID += 1;
        }
        else {
            shaderID = 1;
        }

        prog.setUniform("shaderID", shaderID);
    }

    
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //set up material for teapot
    prog.setUniform("Material.Kd", 0.2f, 0.55f, 0.9f);
    prog.setUniform("Material.Ka", 0.1f, 0.25f, 0.45f);
    prog.setUniform("Material.Ks", 0.6f, 0.8f, 1.0f);

    //set model matrix for teapot
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 0.0f, -2.0f));
    model = glm::rotate(model, glm::radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angle), vec3(0.0f, 0.0f, 1.0f));

    //render teapot
    setMatrices();
    teapot.render();

    //set up material for torus
    prog.setUniform("Material.Kd", 0.2f, 0.55f, 0.9f);
    prog.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
    prog.setUniform("Material.Ka", 0.2f * 0.3f, 0.55f * 0.3f, 0.9f * 0.3f);

    //set model matrix for torus
    model = mat4(1.0f);
    model = glm::translate(model, vec3(-1.0f, 0.75f, 3.0f));
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angle), vec3(0.0f, 0.0f, 1.0f));

    //render torus
    setMatrices();
    torus.render();

    //set up material for plane (flat surface)
    prog.setUniform("Material.Kd", 0.7f, 0.7f, 0.7f);
    prog.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
    prog.setUniform("Material.Ka", 0.2f * 0.3f, 0.55f * 0.3f, 0.9f * 0.3f);

    //set model matrix for plane
    model = mat4(1.0f);

    //render plane
    setMatrices();
    plane.render();
}

void SceneBasic_Uniform::setMatrices()
{
    mat4 mv = view * model; //create a model view matrix
    
    prog.setUniform("ModelViewMatrix", mv); //set the uniform for the model view matrix
    
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]))); //set the uniform for normal matrix
    
    prog.setUniform("MVP", projection * mv); //set the model view matrix with mv and projection matrix
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}
