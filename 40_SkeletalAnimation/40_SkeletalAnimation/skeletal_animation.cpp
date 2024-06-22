#define _CRT_SECURE_NO_WARNINGS


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
//#include <learnopengl/model.h>
#include <learnopengl/animator.h>
#include <learnopengl/model_animation.h>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "keyframe.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <queue>

#include <algorithm> 

#include <mass.h>
#include <plane.h>

#include <map>
#include <string>
#include <random>
#include <time.h>
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
using namespace std;

#include <ft2build.h>
#include FT_FREETYPE_H

#include <shader.h>
#include <cube.h>
#include <arcball.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(char const* path);
void initKeyframes();
void updateAnimData();
void particleInit();
void updateAnimData2();

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void RenderText(Shader* shader, std::string text, float x, float y, float scale, glm::vec3 color);
int current_distance = 0;
string dist_to_print = "0";
int current_speed = 0;
bool speed_changed = true;
float current_position = 0;
string speed_to_print = "0";
int endornot = 0;
int final_dist = 0;
int no_of_life = 3;
int rockpaperscissors = 0;
int whatdidyouchoose = 0;
int range = 3;
int opponent = 0;
float time_for_sound = 0.0f;
double time_for_rockpaperscissors = 0;
double time_for_normalrendering = 0;
void loadTexture2();
int textinitializing();
void initializing_init();
void normalscreenrendering_text_sound();
void rockpaperscissorsscreenrendering();
void rockpaperscissorsscreenrendering_result();
void academictranscriptscreenrendering();
// void draw_yonsei_image(Shader& shader, static unsigned int yonsei_texture);
static unsigned int yonsei_texture; // Array of texture ids.

unsigned int loadCubemap(vector<std::string> faces);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
unsigned int VAO[2], VBO[2], EBO;

Shader* shader = NULL;
Shader* yonsei_shader = NULL;

// camera
Camera camera(glm::vec3(-60.0f, 2.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


// for camera
glm::vec3 cameraOrigPos(0.0f, 0.0f, 9.0f);
glm::vec3 cameraPos(0.0f, 0.0f, 9.0f); // current position of camera
glm::vec3 modelPan(0.0f, 0.0f, 0.0f);  // model panning vector

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// for keyframe animation
enum RenderMode { INIT, ANIM, STOP };
RenderMode renderMode;                  // current rendering mode
float beginT;                           // animation beginning time (in sec)
float timeT;                            // current time (in sec)
float animEndTime = 5.0f;               // ending time of animation (in sec)         // current translation factors
float yAngle;           // current rotation factors
KeyFraming yRKF(5);   // rotation keyframes

// for particle
Mass* particle[100] = { 0 };                                 // particle
float massM = 1.0f;                             // mass of the particle
float particleInitY = 10.0f;                     // initial particle's Y
Shader* particleShader = NULL;

// for ground
Plane* ground;                                  // ground
float groundY = 0.0f;                            // ground's y coordinates
float groundScale = 20.0f;                        // ground's scale (x and z)
Shader* groundShader = NULL;

RenderMode particle_Mode = ANIM;                  // current rendering mode
float particle_time = 0.0f;                     // current time (in sec)
float deltaT = 1.0f / 30.0f;              // time interval between two consecutive frames (in sec)
int nFrame = 0;

float planeVertices[] = {
    // positions          // normals         // texture coords
     80.0f, -0.5f,  30.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
    -80.0f, -0.5f,  30.0f,  0.0f, 1.0f, 0.0f,  0.0f,  0.0f,
    -80.0f, -0.5f, -30.0f,  0.0f, 1.0f, 0.0f,  0.0f, 10.0f,

     80.0f, -0.5f,  30.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
    -80.0f, -0.5f, -30.0f,  0.0f, 1.0f, 0.0f,  0.0f, 10.0f,
     80.0f, -0.5f, -30.0f,  0.0f, 1.0f, 0.0f, 10.0f, 10.0f
};

float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};



int main()
{

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    string sourceDirStr = "(Project폴더가 있는 절대경로로 수정 필요)/40_SkeletalAnimation/40_SkeletalAnimation";
    string modelDirStr = "(Project폴더가 있는 절대경로로 수정 필요)/data";

    // build and compile shaders
    // -------------------------
    Shader ourShader("1.model_loading.vs", "1.model_loading.fs");
    Shader skyboxShader("6.1.skybox.vs", "6.1.skybox.fs");
    //string vs = sourceDirStr + "/6.1.skybox.vs";
    //string fs = sourceDirStr + "/6.1.skybox.fs";
    //Shader skyboxShader(vs.c_str(), fs.c_str());

    // load models
    // -----------
    string luPath = modelDirStr + "/backpack/ludeng.gltf";
    string buidingPath = modelDirStr + "/backpack/building.obj";
    string eaglePath = modelDirStr + "/eagle/dae/eagle.gltf";
    Model ourModel(luPath);
    Model buildingModel(buidingPath);
    Model eagleModel(eaglePath);

    // build and compile shaders
// -------------------------
    string vs_skel = sourceDirStr + "/skel_anim.vs";
    string fs_skel = sourceDirStr + "/skel_anim.fs";
    Shader skelShader(vs_skel.c_str(), fs_skel.c_str());

    // load models
    string runPath1 = modelDirStr + "/cs/dae/Crouched Walking.dae";
    string runPath2 = modelDirStr + "/cs/dae/Slow Run.dae";
    string runPath3 = modelDirStr + "/cs/dae/Fast Run.dae";

    Model runModel(runPath1);
    Animation anim(runPath1, &runModel);
    Animator animator(&anim);

    Model runModel1(runPath1);
    Animation anim1(runPath1, &runModel1);
    Animator animator1(&anim1);

    Model runModel2(runPath2);
    Animation anim2(runPath2, &runModel2);
    Animator animator2(&anim);

    Model runModel3(runPath3);
    Animation anim3(runPath3, &runModel3);
    Animator animator3(&anim3);

    // shader loading and compile (by calling the constructor)
    groundShader = new Shader("basic_lighting.vs", "basic_lighting.fs");
    particleShader = new Shader("particle.vs", "particle.fs");

    // projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
        (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    groundShader->use();
    groundShader->setMat4("projection", projection);
    particleShader->use();
    particleShader->setMat4("projection", projection);


    // particle initialization
    for (int i = 0; i < 100; i++) {
        particle[i] = new Mass(massM);
    }
    // ground initialization
    ground = new Plane(0.0f, 0.0f, 0.0f, groundScale);

    // initialize animation data
    particleInit();
    updateAnimData2();

    textinitializing();
    initializing_init();

    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    string floorPath = modelDirStr + "/textures/brickwall.jpg";
    unsigned int floorTexture = loadTexture(floorPath.c_str());
    //unsigned int ludeng1Texture = loadTexture(FileSystem::getPath("resources/textures/brickwall.jpg").c_str());



    vector<std::string> faces
    {
        modelDirStr + "/textures/skybox/right.jpg",
            modelDirStr + "/textures/skybox/left.jpg",
            modelDirStr + "/textures/skybox/top.jpg",
            modelDirStr + "/textures/skybox/bottom.jpg",
            modelDirStr + "/textures/skybox/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    camera.ProcessKeyboard(LEFT, 5.0f);
    camera.ProcessMouseMovement(900.0f, 0.0f);

    // initialize animation data
    initKeyframes();
    timeT = 0.0f;
    updateAnimData();
    renderMode = INIT;

    renderMode = ANIM;
    beginT = glfwGetTime();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        if (endornot == 0) {
            if (rockpaperscissors == 0) {
                // per-frame time logic
                // --------------------
                float currentFrame = static_cast<float>(glfwGetTime());
                if (current_speed != 0)
                    deltaTime = currentFrame - lastFrame;
                else
                    deltaTime = 0;
                lastFrame = currentFrame;

                // input
                // -----
                processInput(window);

                if (speed_changed) {
                    speed_changed = false;
                    if (current_speed == 1 || current_speed == 2) {
                        runModel = runModel1;
                        anim = anim1;
                        animator = animator1;
                    }
                    if (current_speed == 3) {
                        runModel = runModel2;
                        anim = anim2;
                        animator = animator2;
                    }
                    if (current_speed == 4 || current_speed == 5) {
                        runModel = runModel3;
                        anim = anim3;
                        animator = animator3;
                    }
                }
                animator.UpdateAnimation(deltaTime);

                // render
                // ------
                glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // don't forget to enable shader before setting uniforms
                ourShader.use();
                glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 100.0f, 0.0f));
                ourShader.setMat4("view", view);

                // view/projection transformations
                projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
                view = camera.GetViewMatrix();
                ourShader.setMat4("projection", projection);
                ourShader.setMat4("view", view);

                if (renderMode == ANIM) {
                    float cTime = (float)glfwGetTime(); // current time
                    timeT = cTime - beginT;
                    updateAnimData();
                }

                // render the plane
                glm::mat4 model = glm::mat4(1.0f);
                ourShader.setMat4("model", model);

                // bind diffuse map
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, floorTexture);
                glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);

                glBindVertexArray(planeVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glBindVertexArray(0);

                // building
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(35.0f, -0.5f, 0.0f)); // translate it down so it's at the center of the scene
                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));   // it's a bit too big for our scene, so scale it down
                ourShader.setMat4("model", model);
                buildingModel.Draw(ourShader);

                //eagle 
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(20.0f, 5.0f, 0.0f)); // translate it down so it's at the center of the scene
                model = glm::rotate(model, glm::radians(270.0f) + yAngle, glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));   // it's a bit too big for our scene, so scale it down
                ourShader.setMat4("model", model);
                eagleModel.Draw(ourShader);

                // render the loaded model
                // model = glm::mat4(1.0f);
                for (size_t i = 0; i < 6; i++)
                {
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(0.0f + 2.9f * i, -0.8f, 10.0f)); // translate it down so it's at the center of the scene
                    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));   // it's a bit too big for our scene, so scale it down
                    ourShader.setMat4("model", model);
                    ourModel.Draw(ourShader);
                }

                for (size_t i = 0; i < 6; i++)
                {
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(0.0f + 2.9f * i, -0.8f, -10.0f)); // translate it down so it's at the center of the scene
                    model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));   // it's a bit too big for our scene, so scale it down
                    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    ourShader.setMat4("model", model);
                    ourModel.Draw(ourShader);
                }

                skelShader.use();
                // view/projection transformations
                projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
                view = camera.GetViewMatrix();
                skelShader.setMat4("projection", projection);
                skelShader.setMat4("view", view);

                auto transforms = animator.GetFinalBoneMatrices();
                for (int i = 0; i < transforms.size(); ++i)
                    skelShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

                // player
                current_position += current_speed * deltaT * 0.05;
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(-60.0f + current_position, -0.4f, 0.0f)); // translate it down so it's at the center of the scene
                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));   // it's a bit too big for our scene, so scale it down
                skelShader.setMat4("model", model);
                runModel.Draw(skelShader);
                camera.ProcessKeyboard(FORWARD, deltaT * current_speed * 0.02);


                if (particle_Mode == ANIM) {
                    updateAnimData2();
                }

                // draw ground
                groundShader->use();
                groundShader->setMat4("view", view);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, groundY));
                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                groundShader->setMat4("model", model);
                //ground->draw(groundShader);

                // draw particle
                particleShader->use();
                particleShader->setMat4("view", view);
                model = glm::mat4(1.0);
                model = glm::translate(model, glm::vec3(-30.0f, -0.4f, -10.0f)); // translate it down so it's at the center of the scene
                model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));   // it's a bit too big for our scene, so scale it down
                particleShader->setMat4("model", model);
                for (int i = 0; i < 100; i++) {
                    particle[i]->draw(particleShader, 1.0f, 1.0f, 1.0f);
                }


                // draw skybox as last        
                glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
                skyboxShader.use();
                view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
                skyboxShader.setMat4("view", view);
                skyboxShader.setMat4("projection", projection);
                // skybox cube
                glBindVertexArray(skyboxVAO);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                glBindVertexArray(0);
                glDepthFunc(GL_LESS); // set depth function back to default 

                normalscreenrendering_text_sound();
            }
            else if (rockpaperscissors == 1) {
                rockpaperscissorsscreenrendering();
            }
            else if (rockpaperscissors == 2) {
                rockpaperscissorsscreenrendering_result();
            }
        }
        else {
            academictranscriptscreenrendering();
        }
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
// -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glDeleteVertexArrays(1, &skyboxVAO);

    glDeleteBuffers(1, &skyboxVBO);
    glfwTerminate();
    return 0;
}

void initKeyframes() {

    // y-rotation keyframes
    yRKF.setKey(0, 0, 0.0);
    yRKF.setKey(1, animEndTime / 4 * 1, glm::radians(90.0));
    yRKF.setKey(2, animEndTime / 4 * 2, glm::radians(180.0));
    yRKF.setKey(3, animEndTime / 4 * 3, glm::radians(90.0));
    yRKF.setKey(4, animEndTime, glm::radians(0.0));
}

void updateAnimData() {
    if (timeT > animEndTime) {
        renderMode = STOP;
        timeT = animEndTime;
    }
    yAngle = yRKF.getValLinear(timeT);
}

void particleInit() {
    for (int i = 0; i < 100; i++) {
        particle[i]->setPosition(0.0, particleInitY, 0.0);
        particle[i]->setVelocity(0.0, 0.0, 0.0);
        particle[i]->setAcceleration(0.0, 0.0, 0.0);
    }
}

void updateAnimData2() {
    if (particle_Mode == INIT) {
        particle_Mode = ANIM;
        particle_time = 0.0f;
        nFrame = 0;
    }
    else if (particle_Mode == ANIM) {

        float pi = 3.14159265359;

        for (int i = 0; i < 100; i++) {
            if (nFrame == 0) particle[i]->euler(timeT, deltaT, 30.0 * cos(2 * pi * i / 100), 200.0, 30 * sin(2 * pi * i / 100));
            //else if (nFrame == 80) particle[i]->euler(timeT, deltaT, 10.0 * cos(2 * pi * i / 100), 400.0, 10 * sin(2 * pi * i / 100));
            else particle[i]->euler(timeT, deltaT, 0.0, 0.0, 0.0);
        }
        particle_time = particle_time + deltaT;
        nFrame++;

        if (particle[0]->p[1] < groundY) {
            particle[0]->p[1] = groundY;
            particle_Mode = STOP;
            updateAnimData2();
        }

    }
    else if (particle_Mode == STOP) {
        particle_Mode = ANIM;
        particle_time = 0.0;
        nFrame = 0;
        particleInit();
        updateAnimData2();
    }
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    /*
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    */
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}




unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        if (current_speed < 5) {
            current_speed++;
            if (current_speed == 3 || current_speed == 4)speed_changed = true;
            speed_to_print = to_string(current_speed);
            time_for_normalrendering = glfwGetTime();
        }
    }
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        if (current_speed > 0) {
            current_speed--;
            if (current_speed == 2 || current_speed == 3)speed_changed = true;
            speed_to_print = to_string(current_speed);
            time_for_normalrendering = glfwGetTime();
        }
    }
    else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        whatdidyouchoose = 1;
        rockpaperscissors = 2;
        srand(time(NULL));
        opponent = rand() % range + 1;
        time_for_rockpaperscissors = glfwGetTime();
    }
    else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        whatdidyouchoose = 2;
        rockpaperscissors = 2;
        srand(time(NULL));
        opponent = rand() % range + 1;
        time_for_rockpaperscissors = glfwGetTime();
    }
    else if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        whatdidyouchoose = 3;
        rockpaperscissors = 2;
        srand(time(NULL));
        opponent = rand() % range + 1;
        time_for_rockpaperscissors = glfwGetTime();
    }
}


// render line of text
// -------------------
void RenderText(Shader* shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state   
    shader->use();
    glUniform3f(glGetUniformLocation(shader->ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO[0]);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void loadTexture2() {

    // Create texture ids.
    glGenTextures(1, &yonsei_texture);

    glActiveTexture(GL_TEXTURE1);

    // All upcomming GL_TEXTURE_2D operations now on "texture" object
    glBindTexture(GL_TEXTURE_2D, yonsei_texture);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* image = stbi_load("yonsei.jpg", &width, &height, &nrChannels, 0);
    if (!image) {
        printf("texture %s loading error ... \n", "yonsei.jpg");
    }
    else printf("texture %s loaded\n", "yonsei.jpg");

    GLenum format;
    if (nrChannels == 1) format = GL_RED;
    else if (nrChannels == 3) format = GL_RGB;
    else if (nrChannels == 4) format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, yonsei_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
}

int textinitializing() {
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // OpenGL state
    // ------------
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // compile and setup the shader
    // ----------------------------
    shader = new Shader("textshader.vs", "textshader.fs");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    shader->use();
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // yonsei shader
    yonsei_shader = new Shader("yonsei.vs", "yonsei.fs");

    // 만약 object 더 필요하다면 여기에서 늘리기
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // FreeType
    // --------
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, "Lora-Regular.ttf", 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return 0;
}

void initializing_init() {
    glBindVertexArray(VAO[1]);

    GLfloat vertices[12] = {
    40.0f,430.0f,0.0f,
    140.0f,430.0f,0.0f,
    140.0f,530.0f,0.0f,
    40.0f,530.0f,0.0f
    };

    GLfloat texcoords[8] = {
        0.0f,0.0f,
        1.0f,0.0f,
        1.0f,1.0f,
        0.0f,1.0f
    };

    GLuint indices[6] = {
        0,1,2,
        0,2,3
    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 5 * sizeof(GLfloat), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * 3 * sizeof(GLfloat), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), 4 * 2 * sizeof(GLfloat), texcoords);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    unsigned int vSize = 4 * 3 * sizeof(GLfloat);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)vSize);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    loadTexture2();
    PlaySound(TEXT("Squid-Game-Robot-Sound-edited.wav"), 0, SND_FILENAME | SND_ASYNC);
}
void normalscreenrendering_text_sound() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT);


    RenderText(shader, "Life left : " + to_string(no_of_life), 25.0f, 550.0f, 0.8f, glm::vec3(255.0f, 255.0f, 255.0f));
    string print_speed_str = speed_to_print;
    RenderText(shader, "Current Speed : " + print_speed_str, 25.0f, 25.0f, 0.8f, glm::vec3(255.0f, 255.0f, 255.0f));
    string printing_str = dist_to_print + "m";
    RenderText(shader, printing_str, 660.0f, 550.0f, 1.0f, glm::vec3(255.0f, 255.0f, 255.0f));
    if (current_distance < 300) {
        if (glfwGetTime() - time_for_normalrendering > 0.5) {
            current_distance = current_distance + current_speed;
            time_for_sound += 0.5;
            //printf("%f", time_for_sound);
            if (current_distance > 300) {
                current_distance = 300;
            }
            dist_to_print = to_string(current_distance);
            time_for_normalrendering = glfwGetTime();
            if (time_for_sound == 5.0f) {
                //printf("%f", time_for_sound);
                if (current_speed != 0) {
                    rockpaperscissors = 1;
                }
            }
            if (time_for_sound == 6.0f) {
                //printf("%f", time_for_sound);
                time_for_sound = 0.0f;
            }
            if (time_for_sound == 0.0f) {
                timeT = 0.0f;
                updateAnimData();
                renderMode = INIT;

                renderMode = ANIM;
                beginT = glfwGetTime();
                PlaySound(TEXT("Squid-Game-Robot-Sound-edited.wav"), 0, SND_FILENAME | SND_ASYNC);
            }
            if (current_distance == 300) {
                final_dist = current_distance;
                endornot = 1;
            }
        }
    }
}

void rockpaperscissorsscreenrendering() {
    time_for_sound = -0.5f;
    //PlaySound(NULL, 0, SND_FILENAME | SND_ASYNC);

    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    //glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderText(shader, "Life left : " + to_string(no_of_life), 25.0f, 550.0f, 0.8f, glm::vec3(1.0f, 1.0f, 1.0f));
    RenderText(shader, "Choose!", 300.0f, 500.0f, 0.8f, glm::vec3(1.0f, 1.0f, 1.0f));
    RenderText(shader, "Press Q for Rock", 250.0f, 400.0f, 0.8f, glm::vec3(1.0f, 1.0f, 1.0f));
    RenderText(shader, "Press W for Paper", 240.0f, 350.0f, 0.8f, glm::vec3(1.0f, 1.0f, 1.0f));
    RenderText(shader, "Press E for Scissors", 220.0f, 300.0f, 0.8f, glm::vec3(1.0f, 1.0f, 1.0f));
    RenderText(shader, "Waiting for your choice...", 180.0f, 160.0f, 0.8f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void rockpaperscissorsscreenrendering_result() {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderText(shader, "Life left : " + to_string(no_of_life), 25.0f, 550.0f, 0.8f, glm::vec3(0.0f, 0.0f, 0.0f));
    if (whatdidyouchoose == 1) {
        RenderText(shader, "You chose Rock", 250.0f, 400.0f, 0.7f, glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else if (whatdidyouchoose == 2) {
        RenderText(shader, "You chose Paper", 250.0f, 400.0f, 0.7f, glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else if (whatdidyouchoose == 3) {
        RenderText(shader, "You chose Scissors", 250.0f, 400.0f, 0.7f, glm::vec3(0.0f, 0.0f, 0.0f));
    }

    if (opponent == 1) {
        RenderText(shader, "The opponent chose Rock", 180.0f, 300.0f, 0.7f, glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else if (opponent == 2) {
        RenderText(shader, "The opponent chose Paper", 180.0f, 300.0f, 0.7f, glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else if (opponent == 3) {
        RenderText(shader, "The opponent chose Scissors", 180.0f, 300.0f, 0.7f, glm::vec3(0.0f, 0.0f, 0.0f));
    }

    if (whatdidyouchoose == opponent) {
        RenderText(shader, "It's a tie! You lose life", 140.0f, 200.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
        if (glfwGetTime() - time_for_rockpaperscissors > 3) {
            no_of_life--;
            if (no_of_life == 0) {
                final_dist = current_distance;
                endornot = 1;
            }
            else {
                rockpaperscissors = 0;
            }
        }
    }
    else {
        if (whatdidyouchoose == 1 && opponent == 2) {
            RenderText(shader, "You lost! You lose life", 160.0f, 200.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
            if (glfwGetTime() - time_for_rockpaperscissors > 3) {
                no_of_life--;
                if (no_of_life == 0) {
                    final_dist = current_distance;
                    endornot = 1;
                }
                else {
                    rockpaperscissors = 0;
                }
            }
        }
        else if (whatdidyouchoose == 1 && opponent == 3) {
            RenderText(shader, "You Win! You don't lose any life", 60.0f, 200.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
            if (glfwGetTime() - time_for_rockpaperscissors > 3) {
                rockpaperscissors = 0;
            }
        }
        else if (whatdidyouchoose == 2 && opponent == 1) {
            RenderText(shader, "You Win! You don't lose any life", 60.0f, 200.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
            if (glfwGetTime() - time_for_rockpaperscissors > 3) {
                rockpaperscissors = 0;
            }
        }
        else if (whatdidyouchoose == 2 && opponent == 3) {
            RenderText(shader, "You lost! You lose life", 160.0f, 200.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
            if (glfwGetTime() - time_for_rockpaperscissors > 3) {
                no_of_life--;
                if (no_of_life == 0) {
                    final_dist = current_distance;
                    endornot = 1;
                }
                else {
                    rockpaperscissors = 0;
                }
            }
        }
        else if (whatdidyouchoose == 3 && opponent == 1) {
            RenderText(shader, "You lost! You lose life", 160.0f, 200.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
            if (glfwGetTime() - time_for_rockpaperscissors > 3) {
                no_of_life--;
                if (no_of_life == 0) {
                    final_dist = current_distance;
                    endornot = 1;
                }
                else {
                    rockpaperscissors = 0;
                }
            }
        }
        else if (whatdidyouchoose == 3 && opponent == 2) {
            RenderText(shader, "You Win! You don't lose any life", 60.0f, 200.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
            if (glfwGetTime() - time_for_rockpaperscissors > 3) {
                rockpaperscissors = 0;
            }
        }
    }
}


void academictranscriptscreenrendering() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    PlaySound(NULL, 0, SND_FILENAME | SND_ASYNC);

    yonsei_shader->use();
    glm::mat4 projection_yonsei = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    glUniformMatrix4fv(glGetUniformLocation(yonsei_shader->ID, "projection_yonsei"), 1, GL_FALSE, glm::value_ptr(projection_yonsei));
    // glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, yonsei_texture);

    glBindVertexArray(VAO[1]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    RenderText(shader, "ACADEMIC TRANSCRIPT", 170.0f, 450.0f, 0.8f, glm::vec3(0.0f, 0.0f, 0.0f));
    RenderText(shader, "20XX1475XX", 670.0f, 450.0f, 0.3f, glm::vec3(0.0f, 0.0f, 0.0f));
    RenderText(shader, "------------------------------------------------------", 0.0f, 400.0f, 1.3f, glm::vec3(0.0f, 0.0f, 0.0f));
    RenderText(shader, "Final Distance = " + to_string(final_dist), 250.0f, 370.0f, 0.5f, glm::vec3(0.0f, 0.0f, 0.0f));
    RenderText(shader, "No. of Life left = " + to_string(no_of_life), 250.0f, 320.0f, 0.5f, glm::vec3(0.0f, 0.0f, 0.0f));
    if (final_dist > 250) { // A
        RenderText(shader, "Final Grade = A", 230.0f, 200.0f, 0.7f, glm::vec3(0.0f, 0.0f, 0.0f));
        RenderText(shader, "Well Done!", 270.0f, 150.0f, 0.7f, glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else if (final_dist > 200) { // B
        RenderText(shader, "Final Grade = B", 230.0f, 200.0f, 0.7f, glm::vec3(0.0f, 0.0f, 0.0f));
        RenderText(shader, "Well Done!", 270.0f, 150.0f, 0.7f, glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else if (final_dist > 100) { // C
        RenderText(shader, "Final Grade = C", 230.0f, 200.0f, 0.7f, glm::vec3(0.0f, 0.0f, 0.0f));
        RenderText(shader, "Try Harder!", 260.0f, 150.0f, 0.7f, glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else if (final_dist > 50) { // D
        RenderText(shader, "Final Grade = D", 230.0f, 200.0f, 0.7f, glm::vec3(0.0f, 0.0f, 0.0f));
        RenderText(shader, "Try Harder!", 260.0f, 150.0f, 0.7f, glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else { // F
        RenderText(shader, "Final Grade = F", 230.0f, 200.0f, 0.7f, glm::vec3(0.0f, 0.0f, 0.0f));
        RenderText(shader, "Take this class again!", 210.0f, 150.0f, 0.7f, glm::vec3(0.0f, 0.0f, 0.0f));
    }

    // 만약 Final Grade A나 B면 Well Done! C나 D면 Try Harder! F면 Take this class again!
    RenderText(shader, "Print ESC Key to end.", 250.0f, 80.0f, 0.5f, glm::vec3(0.0f, 0.0f, 0.0f));
}