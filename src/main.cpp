#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <random>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);

void renderQuad();
void renderCube();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
bool indikator=false;
bool flashLight = false;
bool flashLightKeyPressed = false;

// camera
Camera camera(glm::vec3(8.0f, 3.0f, 10.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
auto currentFrame = (float)glfwGetTime();

float y_pozicija=0;
glm::vec3 modelPozicija(-4.0f, 1.2f, 0.0f);
float ugao = 0.0f;

float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "RG_PROJEKAT", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    //SSAO shader
    Shader shaderGeometryPass("resources/shaders/9.ssao_geometry.vs", "resources/shaders/9.ssao_geometry.fs");
    Shader shaderLightingPass("resources/shaders/9.ssao.vs", "resources/shaders/9.ssao_lighting.fs");
    Shader shaderSSAO("resources/shaders/9.ssao.vs", "resources/shaders/9.ssao.fs");
    Shader shaderSSAOBlur("resources/shaders/9.ssao.vs", "resources/shaders/9.ssao_blur.fs");

    // Shader initialization
    // -------------------------
    Shader osvetljenjeShader("resources/shaders/osvetljenja.vs", "resources/shaders/osvetljenja.fs");
    Shader sunce("resources/shaders/sunce.vs", "resources/shaders/sunce.fs");
    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
    Shader blendingShader("resources/shaders/blending.vs", "resources/shaders/blending.fs");

    Model ourModel("resources/objects/RigModels/Model/model.obj");
    ourModel.SetShaderTextureNamePrefix("material.");

    Model autoModel("resources/objects/auto/model.obj");
    autoModel.SetShaderTextureNamePrefix("material.");

    // Sunce verticess
    float sunceVertices[] = {
            0.5f, 0.7f, 0.5f, 0.0f, 1.0f,
            0.5f, 0.7f, -0.5f, 1.0f, 1.0f,
            -0.5f, 0.7f, -0.5f, 1.0f, 0.0f,

            0.5f, 0.7f, 0.5f, 1.0f, 0.0f,
            -0.5f, 0.7f, -0.5f, 0.0f, 1.0f,
            -0.5f, 0.7f, 0.5f, 0.0f, 0.0f,

            0.5f, -0.7f, 0.5f, 0.0f, 1.0f,
            0.5f, -0.7f, -0.5f, 1.0f, 1.0f,
            -0.5f, -0.7f, -0.5f, 1.0f, 0.0f,

            0.5f, -0.7f, 0.5f, 1.0f, 0.0f,
            -0.5f, -0.7f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.7f, 0.5f, 0.0f, 0.0f,

            1.0f, 0.0f, -1.0f, 0.0f, 1.0f,
            0.5f, 0.7f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.7f, 0.5f, 1.0f, 0.0f,

            1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
            0.5f, 0.7f, 0.5f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

            1.0f, 0.0f, -1.0f, 0.0f, 1.0f,
            0.5f, -0.7f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.7f, 0.5f, 1.0f, 0.0f,

            1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
            0.5f, -0.7f, 0.5f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

            1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            0.5f, 0.7f, 0.5f, 1.0f, 1.0f,
            -0.5f, 0.7f, 0.5f, 1.0f, 0.0f,

            1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            -0.5f, 0.7f, 0.5f, 0.0f, 1.0f,
            -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

            1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            0.5f, -0.7f, 0.5f, 1.0f, 1.0f,
            -0.5f, -0.7f, 0.5f, 1.0f, 0.0f,

            1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            -0.5f, -0.7f, 0.5f, 0.0f, 1.0f,
            -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

            -1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, 0.7f, 0.5f, 1.0f, 1.0f,
            -0.5f, 0.7f, -0.5f, 1.0f, 0.0f,

            -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            -0.5f, 0.7f, -0.5f, 0.0f, 1.0f,
            -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

            -1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.7f, 0.5f, 1.0f, 1.0f,
            -0.5f, -0.7f, -0.5f, 1.0f, 0.0f,

            -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            -0.5f, -0.7f, -0.5f, 0.0f, 1.0f,
            -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

            -1.0f, 0.0f, -1.0f, 0.0f, 1.0f,
            -0.5f, 0.7f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.7f, -0.5f, 1.0f, 0.0f,

            -1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
            0.5f, 0.7f, -0.5f, 0.0f, 1.0f,
            1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

            -1.0f, 0.0f, -1.0f, 0.0f, 1.0f,
            -0.5f, -0.7f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.7f, -0.5f, 1.0f, 0.0f,

            -1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
            0.5f, -0.7f, -0.5f, 0.0f, 1.0f,
            1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    };

    // floor plain coordinates
    float podVertices[] = {
            // positions          // normals          // texture coords
            0.5f,  0.5f,  0.0f,  0.0f, 0.0f, -1.0f,  1.0f,  1.0f,
            0.5f, -0.5f,  0.0f,  0.0f, 0.0f, -1.0f,  1.0f,  0.0f,
            -0.5f, -0.5f,  0.0f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.0f,  0.0f, 0.0f, -1.0f,  0.0f,  1.0f
    };

    unsigned int podIndices[] = {
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
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

    float transparentVertices[] = {
            // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
            0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, 0.0f,  0.0f,  1.0f,  1.0f,

            0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            0.5f, 0.0f,  0.0f,  1.0f,  1.0f,
            0.5f,  1.0f,  0.0f,  1.0f,  0.0f
    };

//    Sunce
    unsigned int sunceVAO, sunceVBO;
    glGenVertexArrays(1, &sunceVAO);
    glGenBuffers(1, &sunceVBO);

    glBindVertexArray(sunceVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sunceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sunceVertices), sunceVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

//    Pod
    unsigned int podVAO, podVBO, podEBO;
    glGenVertexArrays(1, &podVAO);
    glGenBuffers(1, &podVBO);
    glGenBuffers(1, &podEBO);

    glBindVertexArray(podVAO);
    glBindBuffer(GL_ARRAY_BUFFER, podVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(podVertices), podVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, podEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(podIndices), podIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // transparent VAO
    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    vector<std::string> faces
            {
                    FileSystem::getPath("resources/textures/skybox2/posx.jpg"),
                    FileSystem::getPath("resources/textures/skybox2/negx.jpg"),
                    FileSystem::getPath("resources/textures/skybox2/posy.jpg"),
                    FileSystem::getPath("resources/textures/skybox2/negy.jpg"),
                    FileSystem::getPath("resources/textures/skybox2/posz.jpg"),
                    FileSystem::getPath("resources/textures/skybox2/negz.jpg")
            };

    unsigned int cubemapTexture = loadCubemap(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // Sunce Tekstura
    unsigned int sunceTekstura = loadTexture(FileSystem::getPath("resources/textures/SunceTekstura.jpg").c_str());
    sunce.use();
    sunce.setInt("tekstura", 0);


    unsigned int podDiffuseMap = loadTexture(FileSystem::getPath("resources/textures/raskrsnica.jpg").c_str());
    unsigned int podSpecularMap = loadTexture(FileSystem::getPath("resources/textures/raskrsnicaSpecular.png").c_str());

    //stop znak
    unsigned int transparentTexture = loadTexture(FileSystem::getPath("resources/textures/stop.png").c_str());
    blendingShader.use();
    blendingShader.setInt("texture1", 0);

    // configure g-buffer framebuffer
    // ------------------------------
    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gAlbedo;
    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // color + specular color buffer
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // also create framebuffer to hold SSAO processing stage
    // -----------------------------------------------------
    unsigned int ssaoFBO, ssaoBlurFBO;
    glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    unsigned int ssaoColorBuffer, ssaoColorBufferBlur;
    // SSAO color buffer
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    // and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // generate sample kernel
    // ----------------------
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0;

        // scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    // generate noise texture
    // ----------------------
    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    unsigned int noiseTexture; glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // lighting info
    // -------------
    glm::vec3 lightPos = glm::vec3(2.0, 4.0, -2.0);
    glm::vec3 lightColor = glm::vec3(0.7, 0.2, 0.1);

    // shader configuration
    // --------------------
    shaderLightingPass.use();
    shaderLightingPass.setInt("gPosition", 0);
    shaderLightingPass.setInt("gNormal", 1);
    shaderLightingPass.setInt("gAlbedo", 2);
    shaderLightingPass.setInt("ssao", 3);
    shaderSSAO.use();
    shaderSSAO.setInt("gPosition", 0);
    shaderSSAO.setInt("gNormal", 1);
    shaderSSAO.setInt("texNoise", 2);
    shaderSSAOBlur.use();
    shaderSSAOBlur.setInt("ssaoInput", 0);


    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
//        float time = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(indikator == true) {
            // 1. geometry pass: render scene's geometry/color data into gbuffer
            // -----------------------------------------------------------------
            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT,
                                                    0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 model = glm::mat4(1.0f);
            shaderGeometryPass.use();
            shaderGeometryPass.setMat4("projection", projection);
            shaderGeometryPass.setMat4("view", view);
            // room cube
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0, 20.0f, 0.0f));
            model = glm::scale(model, glm::vec3(20.0f));
            shaderGeometryPass.setMat4("model", model);
            shaderGeometryPass.setInt("invertedNormals", 1); // invert normals as we're inside the cube
            renderCube();
            shaderGeometryPass.setInt("invertedNormals", 0);
            // backpack model on the floor
            model = glm::mat4(1.0f);
            model = glm::translate(model, modelPozicija);
            model = glm ::rotate (model, glm::radians(ugao), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(2.4f));
            shaderGeometryPass.setMat4("model", model);
            autoModel.Draw(shaderGeometryPass);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(6.0f, 0.7f, -6.0f));
//            model = glm::rotate(model, glm::radians(-90.0f)?, glm::vec3(1.0, 0.0, 0.0));
            model = glm::scale(model, glm::vec3(0.8f));
            shaderGeometryPass.setMat4("model", model);
            ourModel.Draw(shaderGeometryPass);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);


            // 2. generate SSAO texture
            // ------------------------
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
            glClear(GL_COLOR_BUFFER_BIT);
            shaderSSAO.use();
            // Send kernel + rotation
            for (unsigned int i = 0; i < 64; ++i)
                shaderSSAO.setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
            shaderSSAO.setMat4("projection", projection);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, noiseTexture);
            renderQuad();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);


            // 3. blur SSAO texture to remove noise
            // ------------------------------------
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
            glClear(GL_COLOR_BUFFER_BIT);
            shaderSSAOBlur.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
            renderQuad();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);


            // 4. lighting pass: traditional deferred Blinn-Phong lighting with added screen-space ambient occlusion
            // -----------------------------------------------------------------------------------------------------
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shaderLightingPass.use();
            // send light relevant uniforms
            glm::vec3 lightPosView = glm::vec3(camera.GetViewMatrix() * glm::vec4(lightPos, 1.0));
            shaderLightingPass.setVec3("light.Position", lightPosView);
            shaderLightingPass.setVec3("light.Color", lightColor);
            // Update attenuation parameters
            const float linear = 0.09;
            const float quadratic = 0.032;
            shaderLightingPass.setFloat("light.Linear", linear);
            shaderLightingPass.setFloat("light.Quadratic", quadratic);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, gAlbedo);
            glActiveTexture(GL_TEXTURE3); // add extra SSAO texture to lighting pass
            glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
            renderQuad();
        }

        else {


            glm::vec3 pozicija = glm::vec3(20 * cos(currentFrame / 2), 20 * sin(currentFrame / 2), 0.0f);
            y_pozicija = pozicija.y;

            // Ne pali se lampa ako Sunce ima pozitivnu y osu
            if (y_pozicija > 0)
                flashLight = false;

            osvetljenjeShader.use();
            osvetljenjeShader.setVec3("viewPos", camera.Position);
            osvetljenjeShader.setFloat("material.shininess", 6.0f);
            osvetljenjeShader.setInt("flashLight", flashLight);

            osvetljenjeShader.setVec3("dirLight.direction", 1.0f, -0.5f, 0.0f);
            osvetljenjeShader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
            osvetljenjeShader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
            osvetljenjeShader.setVec3("dirLight.specular", 0.1f, 0.1f, 0.1f);

            osvetljenjeShader.setVec3("pointLight.position", pozicija);
            osvetljenjeShader.setVec3("pointLight.ambient", 0.2f, 0.2f, 0.3f);
            osvetljenjeShader.setVec3("pointLight.diffuse", 1.0f, 1.0f, 1.0f);
            osvetljenjeShader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
            osvetljenjeShader.setFloat("pointLight.constant", 1.0f);
            osvetljenjeShader.setFloat("pointLight.linear", 0.05f);
            osvetljenjeShader.setFloat("pointLight.quadratic", 0.02f);

            osvetljenjeShader.setVec3("spotLight.position", camera.Position);
            osvetljenjeShader.setVec3("spotLight.direction", camera.Front);
            osvetljenjeShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            osvetljenjeShader.setVec3("spotLight.diffuse", 2.0f, 2.0f, 2.0f);
            osvetljenjeShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
            osvetljenjeShader.setFloat("spotLight.constant", 1.0f);
            osvetljenjeShader.setFloat("spotLight.linear", 0.09);
            osvetljenjeShader.setFloat("spotLight.quadratic", 0.032);
            osvetljenjeShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(20.0f)));
            osvetljenjeShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(23.0f)));

            // view/projection transformations
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT,
                                                    0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            osvetljenjeShader.setMat4("projection", projection);
            osvetljenjeShader.setMat4("view", view);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(20.0f));
            osvetljenjeShader.setMat4("model", model);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, podDiffuseMap);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, podSpecularMap);

            glBindVertexArray(podVAO);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            glDisable(GL_CULL_FACE);

            // STOP znak
            blendingShader.use();
            blendingShader.setMat4("projection", projection);
            blendingShader.setMat4("view", view);

            glBindVertexArray(transparentVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, transparentTexture);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(5.0f, 0.0f, 7.0f));
            model = glm::scale(model, glm::vec3(5.0f));
            blendingShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-6.0f, 0.0f, -6.0f));
            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(5.0f));
            blendingShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glDisable(GL_CULL_FACE); ;

            sunce.use();
            sunce.setMat4("projection", projection);
            sunce.setMat4("view", view);
            model = glm::mat4(1.0f);
            model = glm::translate(model, pozicija);
            model = glm::scale(model, glm::vec3(1.2f));
            sunce.setMat4("model", model);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sunceTekstura);
            glBindVertexArray(sunceVAO);
            glDrawArrays(GL_TRIANGLES, 0, 64);


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
            glBindTexture(GL_TEXTURE_2D, 0);
            glDepthFunc(GL_LESS); // set depth function back to default


            osvetljenjeShader.use();
            osvetljenjeShader.setVec3("pointLight.position", glm::vec3(4.0f));
            osvetljenjeShader.setVec3("pointLight.ambient", glm::vec3(0.3f, 0.3f, 0.3f));
            osvetljenjeShader.setVec3("pointLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
            osvetljenjeShader.setVec3("pointLight.specular", glm::vec3(1.0, 1.0, 1.0));
            osvetljenjeShader.setFloat("pointLight.constant", 1.0f);
            osvetljenjeShader.setFloat("pointLight.linear", 0.1f);
            osvetljenjeShader.setFloat("pointLight.quadratic", 0.012f);
            osvetljenjeShader.setVec3("viewPosition", camera.Position);
            osvetljenjeShader.setFloat("material.shininess", 32.0f);

            osvetljenjeShader.setMat4("projection", projection);
            view = camera.GetViewMatrix();
            osvetljenjeShader.setMat4("view", view);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(6.0f, 0.7f, -6.0f));
            model = glm::scale(model, glm::vec3(0.8f));
            osvetljenjeShader.setMat4("model", model);

            ourModel.Draw(osvetljenjeShader);

            model = glm::mat4(1.0f);
            model = glm::translate(model, modelPozicija);
            model = glm::rotate(model, glm::radians(ugao), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(2.4f));
            osvetljenjeShader.setMat4("model", model);

            autoModel.Draw(osvetljenjeShader);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &sunceVAO);
    glDeleteVertexArrays(1, &podVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &sunceVBO);
    glDeleteBuffers(1, &podVBO);
    glDeleteBuffers(1, &podEBO);
    glDeleteBuffers(1, &skyboxVBO);

    // destroy all remaining windows/cursors, free any allocated resources
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && !flashLightKeyPressed)
    {
        if(y_pozicija < 0) {
            flashLight = !flashLight;
            flashLightKeyPressed = true;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE)
    {
        flashLightKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        indikator = true;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_RELEASE)
        indikator=false;

    float velocity = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        if(ugao == 0 and modelPozicija.x < 7.0f)
            modelPozicija.x += 1.0 * velocity;

        if((ugao == 180 or ugao == -180 ) and modelPozicija.x > -7.0f)
            modelPozicija.x -= 1.0 * velocity;

        if(ugao == 90 and modelPozicija.z > -7.0f)
            modelPozicija.z -= 1.0 * velocity;

        if(ugao == -90 and modelPozicija.z < 7.0f)
            modelPozicija.z += 1.0 * velocity;

        if(ugao == 45 and modelPozicija.x < 7.0f and modelPozicija.z > -7.0f){
            modelPozicija.x += 1.0 * velocity;
            modelPozicija.z -= 1.0 * velocity;
        }

        if(ugao == -45 and modelPozicija.x < 7.0f and modelPozicija.z < 7.0f){
            modelPozicija.x += 1.0 * velocity;
            modelPozicija.z += 1.0 * velocity;
        }

        if(ugao == 135 and modelPozicija.x > -7.0f and modelPozicija.z > -7.0f){
            modelPozicija.x -= 1.0 * velocity;
            modelPozicija.z -= 1.0 * velocity;
        }

        if(ugao == -135 and modelPozicija.x > -7.0f and modelPozicija.z < 7.0f){
            modelPozicija.x -= 1.0 * velocity;
            modelPozicija.z += 1.0 * velocity;
        }
    }


    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if(ugao == 0 and modelPozicija.x > -7.0f)
            modelPozicija.x -= 1.0 * velocity;

        if((ugao == 180 or ugao == -180 ) and modelPozicija.x < 7.0f)
            modelPozicija.x += 1.0 * velocity;

        if(ugao == 90 and modelPozicija.z < 7.0f)
            modelPozicija.z += 1.0 * velocity;

        if(ugao == -90 and modelPozicija.z > -7.0f)
            modelPozicija.z -= 1.0 * velocity;

        if(ugao == 45 and modelPozicija.x > -7.0f and modelPozicija.z < 7.0f){
            modelPozicija.x -= 1.0 * velocity;
            modelPozicija.z += 1.0 * velocity;
        }

        if(ugao == - 45 and modelPozicija.x > -7.0f and modelPozicija.z > -7.0f){
            modelPozicija.x -= 1.0 * velocity;
            modelPozicija.z -= 1.0 * velocity;
        }

        if(ugao == 135 and modelPozicija.x < 7.0f and modelPozicija.z < 7.0f){
            modelPozicija.x += 1.0 * velocity;
            modelPozicija.z += 1.0 * velocity;
        }

        if(ugao == -135 and modelPozicija.x < 7.0f and modelPozicija.z > -7.0f){
            modelPozicija.x += 1.0 * velocity;
            modelPozicija.z -= 1.0 * velocity;
        }
    }


    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        if(ugao < 180.0f)
            ugao+=45.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        if(ugao > -180.0f)
            ugao-=45.0f;
    }
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
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

    lastX = (float)xpos;
    lastY = (float)ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll((float)yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = GL_RED;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, (GLint)format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
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

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
                // back face
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
                // front face
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                // left face
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                // right face
                1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
                // bottom face
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                // top face
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
                1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}


// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}