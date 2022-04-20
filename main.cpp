#include <Camera.h>
#include <Shader.h>
#include <Model.h>
#include <FileSystem.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <map>
#include <iostream>
#include <math.h>
#include <random>
#include <stddef.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
// 鼠标回调函数, xpos和ypos是鼠标当前位置
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const * path, bool gammaCorrection = false);
void renderSphere();

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f, lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

int main()
{
  // 初始化和配置
  // ---------------------------------------------------------------------------
  // 初始化
  glfwInit();
  // 这里基于OpenGL版本3.4, 将主版本和次版本都设置为3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // 我们同样明确告诉GLFW我们使用的是核心模式(Core-profile)。明确告诉GLFW我们需要使用核心模式意味着我们只能使用OpenGL功能的一个子集（没有我们已不再需要的向后兼容特性
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // 如果使用的是Mac OS X系统，你还需要加下面这行代码到你的初始化代码中这些配置才能起作用
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // 创建窗口
  // ---------------------------------------------------------------------------
  // 创建窗口对象, 参数分别是长、宽、名称, 后面两个参数暂时忽略
  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  // 设置这个窗口为上下文
  glfwMakeContextCurrent(window);
  // 回调函数, 注册这个函数，告诉GLFW我们希望每当窗口调整大小的时候调用这个函数
  // 这个函数将窗口大小和视口大小保持一致
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  // 设置接受鼠标输入, 并且在窗口不显示光标
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // 调用OpenGL函数之前需要初始化glad2
  // ---------------------------------------------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // 启用深度测试
  glEnable(GL_DEPTH_TEST);

  // 创建着色器
  Shader shader("../shader/pbr.vs", "../shader/pbr.fs");

  shader.use();
  shader.setInt("albedoMap", 0);
  shader.setInt("normalMap", 1);
  shader.setInt("metallicMap", 2);
  shader.setInt("roughnessMap", 3);
  shader.setInt("aoMap", 4);


  unsigned int albedo    = loadTexture(FileSystem::getPath("resource/texture/pbr/rusted_iron/albedo.png").c_str());
  unsigned int normal    = loadTexture(FileSystem::getPath("resource/texture/pbr/rusted_iron/normal.png").c_str());
  unsigned int metallic  = loadTexture(FileSystem::getPath("resource/texture/pbr/rusted_iron/metallic.png").c_str());
  unsigned int roughness = loadTexture(FileSystem::getPath("resource/texture/pbr/rusted_iron/roughness.png").c_str());
  unsigned int ao        = loadTexture(FileSystem::getPath("resource/texture/pbr/rusted_iron/ao.png").c_str());

  // lights
  glm::vec3 lightPositions[] = {
    glm::vec3(0.0f, 0.0f, 10.0f),
  };
  glm::vec3 lightColors[] = {
    glm::vec3(150.0f, 150.0f, 150.0f),
  };
  int nrRows = 7;
  int nrColumns = 7;
  float spacing = 2.5;

  glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
  shader.use();
  shader.setMat4("projection", projection);

  // 保持窗口打开, 接受用户输入, 不断绘制
  // ---------------------------------------------------------------------------
  while (!glfwWindowShouldClose(window))
  {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // 接受键盘输入
    processInput(window);

    // 渲染指令
    // -------
    // 清空颜色缓冲并填充为深蓝绿色
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    // 清除深度缓冲
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    glm::mat4 view = camera.GetViewMatrix();
    shader.setMat4("view", view);
    shader.setVec3("camPos", camera.Position);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, albedo);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, metallic);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, roughness);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, ao);

    glm::mat4 model = glm::mat4(1.0f);
    // render nrRows * nrColumns spheres
    for (int row = 0; row < nrRows; ++row)
    {
      for (int col = 0; col < nrColumns; ++col)
      {
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(
          (col - (nrColumns / 2)) * spacing,
          (row - (nrRows / 2)) * spacing,
          0.0f
        ));
        shader.setMat4("model", model);
        renderSphere();
      }
    }

    // render light
    for (size_t i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
    {
      glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
      newPos = lightPositions[i];
      shader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
      shader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

      model = glm::mat4(1.0f);
      model = glm::translate(model, newPos);
      model = glm::scale(model, glm::vec3(0.5f));
      shader.setMat4("model", model);
      renderSphere();
    }

    // 将缓冲区的像素颜色值绘制到窗口
    glfwSwapBuffers(window);
    // 检查有没有触发事件
    glfwPollEvents();
  }
  // 释放资源
  glfwTerminate();
  return 0;
}

// 回调函数, 窗口大小改变时, 创建渲染窗口
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  // 渲染窗口的大小, glViewport函数前两个参数控制窗口左下角的位置。第三个和第四个参数控制渲染窗口的宽度和高度（像素）
  // 图形学里的viewport视口
  glViewport(0, 0, width, height);
}

// 接受键盘输入
void processInput(GLFWwindow *window)
{
  // ESC退出
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  float cameraSpeed = 2.5f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
  if (firstMouse)
  {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }
  
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const *path, bool gammaCorrection)
{
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data)
  {
    GLenum internalFormat;
    GLenum dataFormat;
    if (nrComponents == 1)
    {
      internalFormat = dataFormat = GL_RED;
    }
    else if (nrComponents == 3)
    {
      internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
      dataFormat = GL_RGB;
    }
    else if (nrComponents == 4)
    {
      internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
      dataFormat = GL_RGBA;  // 4通道, 包含alpha通道(透明度)
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, dataFormat == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // 如果是四通道, 则需要将纹理的环绕方式改为GL_CLAMP_TO_EDGE, 这样草的纹理才不会出现白边
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, dataFormat == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
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

unsigned int loadCubemap(std::vector<std::string> faces)
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
      GLenum format;
      if (nrChannels == 1)
        format = GL_RED;
      else if (nrChannels == 3)
        format = GL_RGB;
      else if (nrChannels == 4)
        format = GL_RGBA;  // 4通道, 包含alpha通道(透明度)

      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
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

unsigned int sphereVAO = 0;
unsigned int indexCount = 0;
void renderSphere()
{
  if (sphereVAO == 0)
  {
    glGenVertexArrays(1, &sphereVAO);

    unsigned int vbo, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359;
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
    {
      for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
      {
        float xSegment = (float)x / (float)X_SEGMENTS;
        float ySegment = (float)y / (float)Y_SEGMENTS;
        float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
        float yPos = std::cos(ySegment * PI);
        float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

        positions.push_back(glm::vec3(xPos, yPos, zPos));
        uv.push_back(glm::vec2(xSegment, ySegment));
        normals.push_back(glm::vec3(xPos, yPos, zPos));
      }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
      if (!oddRow)
      {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
          indices.push_back( y      * (X_SEGMENTS + 1) + x);
          indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
        }
      }
      else
      {
        for (int x = X_SEGMENTS; x >= 0; --x)
        {
          indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
          indices.push_back( y      * (X_SEGMENTS + 1) + x);
        }
      }
      oddRow = !oddRow;
    }
    indexCount = static_cast<unsigned int>(indices.size());

    std::vector<float> data;
    for (size_t i = 0; i < positions.size(); ++i)
    {
      data.push_back(positions[i].x);
      data.push_back(positions[i].y);
      data.push_back(positions[i].z);
      if (normals.size() > 0)
      {
        data.push_back(normals[i].x);
        data.push_back(normals[i].y);
        data.push_back(normals[i].z);
      }
      if (uv.size() > 0)
      {
        data.push_back(uv[i].x);
        data.push_back(uv[i].y);
      }
    }
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    float stride = (3 + 3 + 2) * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
  }
  glBindVertexArray(sphereVAO);
  glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}