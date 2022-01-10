#include <iostream>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Camera.h>
#include <Shader.h>
#include <Model.h>


void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
// 鼠标回调函数, xpos和ypos是鼠标当前位置
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
unsigned int loadTexture(char const * path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f, lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


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
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

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
  // ---------------------------------------------------------------------------
  Shader lightShader("../shader/lightShader.vs", "../shader/lightshader.fs");
  Model myModel("../resource/model/nanosuit/nanosuit.obj");

  // 保持窗口打开, 接受用户输入, 不断绘制
  // ---------------------------------------------------------------------------
  while (!glfwWindowShouldClose(window))
  {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // 接受键盘输入
    processInput(window);

    // 渲染指令
    // -------
    // 清空颜色缓冲并填充为深蓝绿色
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    lightShader.use();

    lightShader.setVec3("viewPos", camera.Position);

    // directional light
    lightShader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    lightShader.setVec3("dirLight.ambient", glm::vec3(0.3f, 0.3f, 0.3f));
    lightShader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
    lightShader.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    // spot light
    lightShader.setVec3("spotLight.position", camera.Position);
    lightShader.setVec3("spotLight.direction", camera.Front);
    lightShader.setVec3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
    lightShader.setVec3("spotLight.diffuse", glm::vec3(5.0f, 5.0f, 5.0f));
    lightShader.setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightShader.setFloat("spotLight.constant", 1.0f);
    lightShader.setFloat("spotLight.linear", 0.09f);
    lightShader.setFloat("spotLight.quadratic", 0.032f);
    lightShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    lightShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    lightShader.setFloat("material.shininess", 32.0f);

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    lightShader.setMat4("projection", projection);
    lightShader.setMat4("view", view);

    glm::mat4 model = glm::mat4(1.0f);
    lightShader.setMat4("model", model);
    myModel.Draw(lightShader);

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
