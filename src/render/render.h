#ifndef RENDER_H
#define RENDER_H

#include "vertex_mesh.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 全局渲染资源
extern unsigned int gridVAO, gridVBO;
extern unsigned int shaderProgram;

// 顶点和片段着色器源码
extern const char* vertexShaderSource;
extern const char* fragmentShaderSource;

// 渲染相关函数
void setupShaders();
void setupGrid();
void setupCoordinateSystem();
void renderCoordinateSystem(const glm::mat4& view, const glm::mat4& projection, float cameraDistance);
void renderGrid(float cameraDistance, const glm::vec3& gridColor);
void renderMesh(const Mesh& mesh);
void printOpenGLInfo();

#endif // RENDER_H
