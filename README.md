<!-- TOC -->

- [环境配置](#环境配置)
  - [1. 创建项目](#1-创建项目)
  - [2. 安装依赖](#2-安装依赖)
  - [3. 创建main.cpp文件](#3-创建maincpp文件)
  - [4. 编译](#4-编译)
  - [4. 第一个窗口](#4-第一个窗口)

<!-- /TOC -->

教程: <https://learnopengl-cn.github.io/>

<a id="markdown-环境配置" name="环境配置"></a>
### 环境配置

环境: MacOS  
编辑器: VSCode

<a id="markdown-1-创建项目" name="1-创建项目"></a>
#### 1. 创建项目

新建一个文件夹作为项目路径, 在VSCode里打开, 在这个路径下新建一个include文件夹, 之后会往里面放依赖包

<a id="markdown-2-安装依赖" name="2-安装依赖"></a>
#### 2. 安装依赖

- glfw  
  GLFW是一个专门针对OpenGL的C语言库，它提供了一些渲染物体所需的最低限度的接口。它允许用户创建OpenGL上下文，定义窗口参数以及处理用户输入，这正是我们需要的.  
  `brew install glfw`  
  安装的过程中可以看到安装地址和版本, 例如我安装的时候显示:  
  `/usr/local/Cellar/glfw/3.3.4: 14 files, 520.9KB`
- glad  
  OpenGL是一个规范, 不同开发版本的函数位置不一样, glad可以在编译时找到对应的函数位置  
  glad是在线服务, 需要去网站上下载, 地址是: <https://glad.dav1d.de/>  
  Language选择C/C++, Specification选择OpenGL, API-gl我选择Version3.3, 因为我glfw的版本是3.4, Profile选择Core  
  然后点击GENERATE生成, 下载glad.zip压缩文件到本地, 解压缩, 将include里的glad和KHR两个文件夹复制到项目里的include路径下, 然后将src下的glad.c文件复制到项目根目录下.

<a id="markdown-3-创建maincpp文件" name="3-创建maincpp文件"></a>
#### 3. 创建main.cpp文件

```C++
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    return 0;
}
```
头两行可能会显示红色提示includepath错误, 找不到依赖文件, 我们需要修改配置  
将鼠标放在头两行的红色区域, 会提示`command + .`跳转到配置页去修改  
或者`command + shift + p`, 输入config搜索, 点击`C/C++: Edit Configurations(UI)`  
在include path配置项里, 添加一行: `/usr/local/Cellar/glfw/3.3.4/include`, 也就是上面安装glfw的路径  
compile path选择`/usr/bin/clang++`  
之后在项目根目录下就会多了一个`.vscode/c_cpp_properties.json`的文件, 这就是配置项的json格式, 修改配置可以直接修改这个json文件, 也可以在刚才的UI页面上修改  
然后我们看到main.cpp文件里的红色错误提示没有了, 然后也能做文件和函数跳转了

<a id="markdown-4-编译" name="4-编译"></a>
#### 4. 编译

这里采用cmake来编译  
如何安装这里不表, 可以搜到.  
在根目录下创建CMakeLists.txt, 内容如下(注意修改glfw的路径):
```txt
cmake_minimum_required(VERSION 3.0.0)
project(HelloGL VERSION 0.1.0)

# 使用 C++ 11 标准
set(CMAKE_CXX_STANDARD 11)

# 添加头文件
set(GLAD_H ${PROJECT_SOURCE_DIR}/include)
set(GLFW_H /usr/local/Cellar/glfw/3.3.4/include)
include_directories(${GLAD_H} ${GLFW_H})

# 添加目标链接
set(GLFW_LINK /usr/local/Cellar/glfw/3.3.4/lib/libglfw.3.dylib)
link_libraries(${GLFW_LINK})

# 执行编译命令
set(SOURCES glad.c main.cpp)
add_executable(HelloGL ${SOURCES})

# 链接系统的 OpenGL 框架
if (APPLE)
    target_link_libraries(HelloGL "-framework OpenGL")
endif()

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
```
然后在根目录下创建build文件夹, 在此文件夹下编译:
```shell
$ mkdir build
$ cd build
$ cmake ..
$ make
```
正常情况下就会编译成功, 在路径下就会生成一个可执行文件HelloGL  
如果编译失败, 可能是前面的步骤有问题  
用`./HelloGL`命令执行这个可执行文件, 什么都不会发生  
我们按照教程修改main.cpp文件, 接着往下看...

<a id="markdown-4-第一个窗口" name="4-第一个窗口"></a>
#### 4. 第一个窗口

按照教程一步步修改main.cpp文件, 如下所示意:
```C++
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    // 初始化
    glfwInit();
    // 这里基于OpenGL版本3.4, 将主版本和次版本都设置为3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // 我们同样明确告诉GLFW我们使用的是核心模式(Core-profile)。明确告诉GLFW我们需要使用核心模式意味着我们只能使用OpenGL功能的一个子集（没有我们已不再需要的向后兼容特性
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // 如果使用的是Mac OS X系统，你还需要加下面这行代码到你的初始化代码中这些配置才能起作用
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // 创建窗口对象, 参数分别是长、宽、名称, 后面两个参数暂时忽略
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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

    // 调用OpenGL函数之前需要初始化glad2
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 保持窗口打开, 接受用户输入, 不断绘制
    while(!glfwWindowShouldClose(window))
    {
        // 接受键盘输入
        processInput(window);

        // 渲染指令
        // 清空颜色缓冲并填充为深蓝绿色
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

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
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // 渲染窗口的大小, glViewport函数前两个参数控制窗口左下角的位置。第三个和第四个参数控制渲染窗口的宽度和高度（像素）
    // 图形学里的viewport视口
    glViewport(0, 0, width, height);
}

// 接受ESC键退出
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
```
重新编译和执行:
```shell
$ make
$ ./HelloGL
```
就会出现一个深蓝绿色的窗口, 按ESC键盘还能退出  
环境配置就讲到这里
