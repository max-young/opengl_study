cmake_minimum_required(VERSION 3.0.0)
project(HelloGL VERSION 0.1.0)

# 使用 C++ 11 标准
set(CMAKE_CXX_STANDARD 11)

# 配置文件
configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

# 添加头文件
set(GLAD_H ${PROJECT_SOURCE_DIR}/include)
set(GLFW_H /usr/local/Cellar/glfw/3.3.4/include)
set(ASSIMP_H /usr/local/Cellar/assimp/5.0.1/include)
include_directories(${GLAD_H} ${GLFW_H} ${ASSIMP_H})

# 添加目标链接
set(GLFW_LINK /usr/local/Cellar/glfw/3.3.4/lib/libglfw.3.dylib)
set(ASSIMP_LINK /usr/local/Cellar/assimp/5.0.1/lib/libassimp.5.dylib )
link_libraries(${GLFW_LINK} ${ASSIMP_LINK})

# 执行编译命令
set(SOURCES main.cpp src/glad.c src/stb_image.cpp)
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
