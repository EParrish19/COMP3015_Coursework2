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
        vec4(0.5f, 0.5f, 0.5f, 1.0f));

    float c = 1.65f;

    vec3 lightPos = vec3(0.0f, c * 5.25f, c * 7.5f); //in world coords

    lightFrustum.orient(lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
    lightFrustum.setPerspective(50.0f, 1.0f, 1.0f, 25.0f);
    lightPV = shadowBias * lightFrustum.getProjectionMatrix() * lightFrustum.getViewMatrix();

    prog.setUniform("Light.La", vec3(0.85f));
    prog.setUniform("Light.Ld", vec3(0.85f));
    prog.setUniform("Light.Ls", vec3(0.85f));
    prog.setUniform("ShadowMap", 0);


    //initialize shader ID to phong shading with shadows
    shaderID = 4;
    prog.setUniform("shaderID", shaderID);

    //set up textures
    GLuint metal = Texture::loadTexture("./media/texture/me_textile.png");
    GLuint moss = Texture::loadTexture("./media/texture/moss.png");

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, metal);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, moss);
}

void SceneBasic_Uniform::setupFBO()
{
    GLfloat border[] = { 1.0f, 0.0f, 0.0f, 0.0f };
    //texture for depth buffer
    GLuint depthTex;
    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, shadowMapWidth, shadowMapHeight);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

    //assign depth buffer texture
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthTex);

    //create FBO
    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

    GLenum drawBuffers[] = { GL_NONE };
    glDrawBuffers(1, drawBuffers);

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer is complete.\n");
    }
    else
    {
        printf("Framebuffer is not complete.\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();

        //light frustum rendering
        solidProg.compileShader("shader/solid.vert", GLSLShader::VERTEX);
        solidProg.compileShader("shader/solid.frag", GLSLShader::FRAGMENT);
        solidProg.link();

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

    //timer -= deltaT;
    angle += 0.2f * deltaT;

    //reset angle if a full rotation has been done
    if (angle > glm::two_pi<float>()) {
        angle -= glm::two_pi<float>();
    }

    /*//when the timer hits 0, change the shader ID given to shader
    if (timer <= 0.0f) {
        timer = 10.0f;
        
        if (!(shaderID == 3)) {
            shaderID += 1;
        }
        else {
            shaderID = 1;
        }

        prog.setUniform("shaderID", shaderID);
    }*/

    
}

void SceneBasic_Uniform::render()
{

    prog.use();
    //pass 1 (generate the shadow map from light perspective)
    view = lightFrustum.getViewMatrix();
    projection = lightFrustum.getProjectionMatrix();
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, shadowMapWidth, shadowMapHeight);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass1Index);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.5f, 10.0f);
    drawScene();
    glCullFace(GL_BACK);
    glFlush();
    //spitOutDepthBuffer(); // for getting image of depth buffer

    //pass 2 (render scene normally)
    float c = 2.0f;
    vec3 cameraPos(c * 11.5f * cos(angle), c * 7.0f, c * 11.5f * sin(angle));
    view = glm::lookAt(cameraPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
    prog.setUniform("Light.Position", view * vec4(lightFrustum.getOrigin(), 1.0f));
    projection = glm::perspective(glm::radians(50.0f), (float)width / height, 0.1f, 100.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass2Index);
    drawScene();

    //draw light Frustum
    solidProg.use();
    solidProg.setUniform("Color", vec4(1.0f, 0.0f, 0.0f, 1.0f));
    mat4 mv = view * lightFrustum.getInverseViewMatrix();
    solidProg.setUniform("MVP", projection * mv);
    lightFrustum.render();
}

void SceneBasic_Uniform::drawScene()
{
    vec3 color = vec3(0.2f, 0.5f, 0.9f);
    prog.setUniform("Material.Ka", color * 0.05f);
    prog.setUniform("Material.Kd", color);
    prog.setUniform("Material.Ks", vec3(0.9f, 0.9f, 0.9f));
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices();
    teapot.render();

    prog.setUniform("Material.Ka", color * 0.05f);
    prog.setUniform("Material.Kd", color);
    prog.setUniform("Material.Ks", vec3(0.9f, 0.9f, 0.9f));
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 2.0f, 5.0f));
    model = glm::rotate(model, glm::radians(-45.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices();
    torus.render();

    prog.setUniform("Material.Kd", 0.25f, 0.25f, 0.25f);
    prog.setUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
    prog.setUniform("Material.Ka", 0.05f, 0.05f, 0.05f);
    model = mat4(1.0f);
    setMatrices();
    plane.render();
    
    model = mat4(1.0f);
    model = glm::translate(model, vec3(-5.0f, 5.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), vec3(0.0f, 0.0f, 1.0f));
    setMatrices();
    plane.render();

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 5.0f, -5.0f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices();
    plane.render();
    model = mat4(1.0f);
}

void SceneBasic_Uniform::setMatrices()
{
    mat4 mv = view * model; //create a model view matrix
    
    prog.setUniform("ModelViewMatrix", mv); //set the uniform for the model view matrix
    
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]))); //set the uniform for normal matrix
    
    prog.setUniform("MVP", projection * mv); //set the model view matrix with mv and projection matrix

    prog.setUniform("ShadowMatrix", lightPV * model); // sets shadow matrix
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}
