#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
  // 程序ID
  unsigned int ID;
  // 用着色器语言文件路径构建着色器
  Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath)
  {
    // 1. 从文件路径获取顶点/片段着色器/几何着色器
    // ----------------------------
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    // 保证ifstream对象可以抛出异常
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try
    {
      // 打开文件
      vShaderFile.open(vertexPath);
      fShaderFile.open(fragmentPath);
      gShaderFile.open(geometryPath);
      std::stringstream vShaderStream, fShaderStream, gShaderStream;
      // 读取文件内容到数据流
      vShaderStream << vShaderFile.rdbuf();
      fShaderStream << fShaderFile.rdbuf();
      gShaderStream << gShaderFile.rdbuf();
      // 关闭文件处理器
      vShaderFile.close();
      fShaderFile.close();
      gShaderFile.close();
      // 转换数据流到string
      vertexCode = vShaderStream.str();
      fragmentCode = fShaderStream.str();
      geometryCode = gShaderStream.str();
    }
    catch(std::ifstream::failure e)
    {
      std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    const char* gShaderCode = geometryCode.c_str();

    // 2. 编译着色器
    // ------------
    unsigned int vertex, fragment, geometry;
    // 顶点着色器
    vertex = glCreateShader(GL_VERTEX_SHADER);
    // 第二个参数是源代码字符串数量
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // 打印编译错误(如果有的话)
    checkCompileErrors(vertex, "VERTEX");
    // 片段着色器
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    // 第二个参数是源代码字符串数量
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // 打印编译错误(如果有的话)
    checkCompileErrors(fragment, "FRAGMENT");
    // 几何着色器
    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &gShaderCode, NULL);
    glCompileShader(geometry);
    checkCompileErrors(geometry, "GEOMETRY");
    // 着色器程序
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glAttachShader(ID, geometry);
    glLinkProgram(ID);
    // 打印连接错误(如果有的话)
    checkCompileErrors(ID, "PROGRAM");
    // 删除着色器, 它们已经链接到我们的程序中了, 已经不再需要了
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glDeleteShader(geometry);
  }
  // 激活程序
  void use()
  {
    glUseProgram(ID);
  }
  // uniform工具函数
  void setBool(const std::string& name, bool value) const
  {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
  }
  void setInt(const std::string& name, int value) const
  {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
  }
  void setFloat(const std::string& name, float value) const
  {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
  }
  void setMat4(const std::string& name, glm::mat4 value) const
  {
    unsigned int loc = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
  }
  void setVec3(const std::string& name, glm::vec3 value) const
  {
    unsigned int loc = glGetUniformLocation(ID, name.c_str());
    glUniform3fv(loc, 1, glm::value_ptr(value));
  }

private:
  void checkCompileErrors(unsigned int shader, std::string type)
  {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" <<infoLog << "\n -- ------------------------------ -- " << std::endl;
      }
    }
    else
    {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);
      if (!success)
      {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" <<infoLog << "\n -- ------------------------------ -- " << std::endl;
      }
    }
  }
};
#endif
