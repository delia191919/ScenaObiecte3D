#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif
#include <glm/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types


#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>


gps::SkyBox mySkyBox;

gps::Shader skyboxShader;
// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;

// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 10.0f, 400.0f),
    glm::vec3(0.0f, 0.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.02f;

GLboolean pressedKeys[1024];

// models
gps::Model3D wave2obj;
gps::Model3D wave2obj2;
gps::Model3D teapot;

gps::Model3D felinar1;
gps::Model3D felinar2;
GLfloat angle;


// shaders
gps::Shader myBasicShader;



float waveAnimationTime = 0.0f;  // Timpul pentru animație
float waveSpeed = 0.05f;         // Viteza de mișcare

struct BoundingBox {
    glm::vec3 min; // colțul minim (stânga-jos)
    glm::vec3 max; // colțul maxim (dreapta-sus)
};

glm::vec3 pointLightPosition(10.0f, 10.0f, 10.0f);
glm::vec3 pointLightColor(1.0f, 1.0f, 1.0f);
float pointLightConstant = 1.0f;
float pointLightLinear = 0.09f;
float pointLightQuadratic = 0.032f;

glm::vec3 felinar1SpotlightPosition(25.0f, 26.5f, 10.0f); // Position of felinar1
glm::vec3 felinar1SpotlightDirection(0.0f, -1.0f, 0.0f); // Direction of the spotlight
glm::vec3 felinar1SpotlightColor(0.0f, 0.0f, 1.0f); // Light warm color
float felinar1SpotlightCutOff = glm::cos(glm::radians(12.5f)); // Cutoff angle
float felinar1SpotlightOuterCutOff = glm::cos(glm::radians(15.0f)); // Outer cutoff angle
float felinar1SpotlightConstant = 1.0f;
float felinar1SpotlightLinear = 0.09f;
float felinar1SpotlightQuadratic = 0.032f;


glm::vec3 redAmbientLight(1.0f, 0.9f, 0.5f); // Red color

BoundingBox teapotBoundingBox, waveBoundingBox, waveBoundingBox2;

bool checkCollision(const glm::vec3& position, const BoundingBox& box) {
    bool collides = (position.x >= box.min.x && position.x <= box.max.x &&
        position.y >= box.min.y && position.y <= box.max.y &&
        position.z >= box.min.z && position.z <= box.max.z);

    if (collides) {
        std::cout << "Collision detected at position: " << position.x << ", " << position.y << ", " << position.z << std::endl;
    }

    return collides;
}


GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)


void initSkyBox() {
    std::vector<const GLchar*> faces;
    faces.push_back("skybox/posx.jpg");
    faces.push_back("skybox/negx.jpg");
    faces.push_back("skybox/posy.jpg");
    faces.push_back("skybox/negy.jpg");
    faces.push_back("skybox/posz.jpg");
    faces.push_back("skybox/negz.jpg");
    mySkyBox.Load(faces);
}
void windowResizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}


GLuint depthMapFBO;
GLuint depthMap;
const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

void initDepthMap() {
    glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


glm::mat4 lightSpaceMatrix;


void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}
float lastX = 400, lastY = 300;
bool firstMouse = true;
float pitch = 0.0f, yaw = -90.0f;
float fov = 45.0f;

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    static float sensitivity = 0.1f; // Sensibilitatea rotației
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Invers pentru a evita inversarea axelor
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // Actualizează direcția camerei
    myCamera.rotate(yoffset, xoffset);  // Observă ordinea yaw -> xoffset, pitch -> yoffset
}


void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;

    // Actualizează matricea de proiecție
    projection = glm::perspective(glm::radians(fov),
        (float)myWindow.getWindowDimensions().width /
        (float)myWindow.getWindowDimensions().height,
        0.1f, 20.0f);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}
glm::vec3 potentialPosition = myCamera.getPosition();

void processMovement() {



    //if (!checkCollision(potentialPosition, teapotBoundingBox) &&
    //    !checkCollision(potentialPosition, waveBoundingBox) &&
    //    !checkCollision(potentialPosition, waveBoundingBox2)) {
    //    myCamera.setPosition(potentialPosition); // Setează noua poziție doar dacă nu există coliziune
    //}
    //
    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);  // Mișcare înainte
    }
    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);  // Mișcare înapoi
    }
    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);  // Mișcare stânga
    }
    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);  // Mișcare dreapta
    }

    // Mișcare pe verticală
    if (pressedKeys[GLFW_KEY_UP]) {
        myCamera.move(gps::MOVE_UP, cameraSpeed);  // Mișcare sus
    }
    if (pressedKeys[GLFW_KEY_DOWN]) {
        myCamera.move(gps::MOVE_DOWN, cameraSpeed);  // Mișcare jos
    }
 
    
    // Rotește scena
    if (pressedKeys[GLFW_KEY_R]) {
        angle += 1.0f; // Rotește scena spre dreapta
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (pressedKeys[GLFW_KEY_F]) {
        angle -= 1.0f; // Rotește scena spre stânga
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    // Scade dimensiunea modelului
    if (pressedKeys[GLFW_KEY_Z]) {
        model = glm::scale(model, glm::vec3(0.95f, 0.95f, 0.95f));
    }
    // Crește dimensiunea modelului
    if (pressedKeys[GLFW_KEY_X]) {
        model = glm::scale(model, glm::vec3(1.05f, 1.05f, 1.05f));
    }
    if (pressedKeys[GLFW_KEY_1]) {    
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (pressedKeys[GLFW_KEY_2]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }
    if (pressedKeys[GLFW_KEY_3]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }



    // Actualizează matricea de vizualizare și normalMatrix
    view = myCamera.getViewMatrix();
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
}



void initOpenGLWindow() {
    myWindow.Create(1024, 768, "OpenGL Project Core");
    std::cout << "Window width: " << myWindow.getWindowDimensions().width << std::endl;
    std::cout << "Window height: " << myWindow.getWindowDimensions().height << std::endl;

}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetScrollCallback(myWindow.getWindow(), scrollCallback);
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initOpenGLState() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

}

void initModels() {
    teapot.LoadModel("models/plaja/plajabuna.obj");
    std::cout << "Model loaded successfully!" << std::endl;

    wave2obj.LoadModel("models/plaja/wavebun.obj");
    std::cout << "New model 'wave2obj.obj' loaded successfully!" << std::endl;

    wave2obj.LoadModel("models/plaja/wavebun2.obj");
    std::cout << "New model 'wave2obj.obj' loaded successfully!" << std::endl;

    felinar1.LoadModel("models/plaja/felinar1.obj");
    std::cout << "New model 'wave2obj.obj' loaded successfully!" << std::endl;

    felinar2.LoadModel("models/plaja/felinar2.obj");
    std::cout << "New model 'wave2obj.obj' loaded successfully!" << std::endl;

    teapotBoundingBox = { glm::vec3(.0f, 0.0f,0.0f), glm::vec3(0.0f, 0.0f, 0.0f) };
    waveBoundingBox = { glm::vec3(-2.0f, 0.0f, -2.0f), glm::vec3(2.0f, 1.0f, 2.0f) };
    waveBoundingBox2 = { glm::vec3(-2.0f, 0.0f, -2.0f), glm::vec3(2.0f, 1.0f, 2.0f) };

}


void initShaders() {
    myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();

}

void initUniforms() {
    myBasicShader.useShaderProgram();

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 20.0f);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //set the light direction (direction towards the light)
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
    // send light dir to shader
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

    //set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    // send light color to shader
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    glm::vec3 redAmbientLight(1.0f, 1.0f, 1.0f); // Red color
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "ambientLightColor"), 1, &redAmbientLight[0]);


}

void setPointLightUniforms(gps::Shader& shader) {
    shader.useShaderProgram();
    glUniform3fv(glGetUniformLocation(shader.shaderProgram, "pointLightPosition"), 1, glm::value_ptr(pointLightPosition));
    glUniform3fv(glGetUniformLocation(shader.shaderProgram, "pointLightColor"), 1, glm::value_ptr(pointLightColor));
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "pointLightConstant"), pointLightConstant);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "pointLightLinear"), pointLightLinear);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "pointLightQuadratic"), pointLightQuadratic);
    glUniform3fv(glGetUniformLocation(shader.shaderProgram, "viewPos"), 1, glm::value_ptr(myCamera.getPosition()));
}
void setSpotlightUniforms(gps::Shader& shader) {
    shader.useShaderProgram();
    glUniform3fv(glGetUniformLocation(shader.shaderProgram, "spotlight.position"), 1, glm::value_ptr(felinar1SpotlightPosition));
    glUniform3fv(glGetUniformLocation(shader.shaderProgram, "spotlight.direction"), 1, glm::value_ptr(felinar1SpotlightDirection));
    glUniform3fv(glGetUniformLocation(shader.shaderProgram, "spotlight.color"), 1, glm::value_ptr(felinar1SpotlightColor));
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "spotlight.cutOff"), felinar1SpotlightCutOff);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "spotlight.outerCutOff"), felinar1SpotlightOuterCutOff);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "spotlight.constant"), felinar1SpotlightConstant);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "spotlight.linear"), felinar1SpotlightLinear);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "spotlight.quadratic"), felinar1SpotlightQuadratic);
}

void renderTeapot(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));
    glm::mat4 tModel = model * scaleMatrix;
    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(tModel));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    teapot.Draw(shader);
}

void renderWave2Obj(gps::Shader shader) {
    // Selectează programul shader activ
    shader.useShaderProgram();

    // Calculăm deplasarea pe axa X pe baza unui sinus care oscilează între -1 și 1
    float displacement = sin(waveAnimationTime) * 0.05f;  // 2.0f este amplitudinea mișcării

    // Creăm matricea de transformare
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(displacement, 0.0f, 0.0f));
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));
    glm::mat4 tModel = model * translationMatrix * scaleMatrix;  // Aplicăm translația și scalarea

    // Trimite matricea de model către shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(tModel));

    // Trimite matricea de normale către shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // Desenează obiectul wave
    wave2obj.Draw(shader);
}

void renderWave2Obj2(gps::Shader shader) {
    // Selectează programul shader activ
    shader.useShaderProgram();

    // Calculăm deplasarea pe axa X pe baza unui sinus care oscilează între -1 și 1
    float displacement = sin(waveAnimationTime) * 0.05f;  // 2.0f este amplitudinea mișcării

    // Creăm matricea de transformare
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(displacement, 0.0f, 0.0f));
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));
    glm::mat4 tModel = model * translationMatrix * scaleMatrix;  // Aplicăm translația și scalarea

    // Trimite matricea de model către shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(tModel));

    // Trimite matricea de normale către shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // Desenează obiectul wave
    wave2obj2.Draw(shader);
}
void renderFelinar(gps::Shader shader) {
    // Selectează programul shader activ
    shader.useShaderProgram();

    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));
    glm::mat4 tModel = model * scaleMatrix;

    // Trimite matricea de model către shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(tModel));

    // Trimite matricea de normale către shader
    //glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    //glUniform1i(glGetUniformLocation(shader.shaderProgram, "isEmissive"), GL_TRUE);

    //// Culoare emisivă (galbenă pentru felinar)
    //glm::vec3 emissiveColor(1.0f, 0.8f, 0.0f); // galben
    //glUniform3fv(glGetUniformLocation(shader.shaderProgram, "emissiveColor"), 1, glm::value_ptr(emissiveColor));
    //// Desenează felinarul
    myBasicShader.useShaderProgram();
    setSpotlightUniforms(myBasicShader);
    felinar1.Draw(shader);
    felinar2.Draw(shader);
    /*glUniform1i(glGetUniformLocation(shader.shaderProgram, "isEmissive"), GL_FALSE);*/
}
void setAmbientLightUniforms(gps::Shader& shader) {
    shader.useShaderProgram();
    GLuint shaderProgram = shader.shaderProgram;
    glUniform3fv(glGetUniformLocation(shaderProgram, "ambientLightColor"), 1, glm::value_ptr(redAmbientLight));
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setAmbientLightUniforms(myBasicShader);
    setPointLightUniforms(myBasicShader);
    setSpotlightUniforms(myBasicShader);
    //render the scene
    waveAnimationTime += waveSpeed;  // Poți ajusta viteza mișcării
    if (waveAnimationTime > glm::two_pi<float>()) {
        waveAnimationTime -= glm::two_pi<float>();  // Resetează timpul pentru a preveni overflow-ul
    }
    // render the teapot
    renderTeapot(myBasicShader);
    renderWave2Obj(myBasicShader);
    renderWave2Obj2(myBasicShader);
    renderFelinar(myBasicShader);
    mySkyBox.Draw(skyboxShader, view, projection);

}
void renderSceneFromLight(gps::Shader& depthShader) {
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    float near_plane = 1.0f, far_plane = 7.5f;
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightDir * -10.0f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;

    depthShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthShader.shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    renderScene(); // Render your scene here
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderSceneWithShadows(gps::Shader& shader) {
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glUniform1i(glGetUniformLocation(shader.shaderProgram, "shadowMap"), 1);

    renderScene(); // Render your scene here
}
void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char* argv[]) {

    try {
        initOpenGLWindow();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Setează poziția camerei pentru a fi în mijlocul modelului
    glm::vec3 modelCenter(0.0f, 0.0f, 0.0f); // Presupunem că modelul este centrat în jurul originii
    float distanceFromModel = 1.0f;  // Distanța camerei față de model
    glm::vec3 cameraPosition = modelCenter + glm::vec3(0.0f, 0.0f, distanceFromModel);
    myCamera = gps::Camera(cameraPosition, modelCenter, glm::vec3(0.0f, 1.0f, 0.0f));

    initOpenGLState();
    initModels();
    initShaders();
    initSkyBox();
    initUniforms();
    initDepthMap();
    setWindowCallbacks();

    glCheckError();
    gps::Shader depthShader;
    depthShader.loadShader("shaders/depthShader.vert", "shaders/depthShader.frag");

    // application loop
    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
        renderScene();
        renderSceneFromLight(depthShader);
        renderSceneWithShadows(myBasicShader);

        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());

        glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}
