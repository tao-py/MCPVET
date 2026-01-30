#ifndef COORDINATE_SYSTEM_H
#define COORDINATE_SYSTEM_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/vector_float2.hpp>

// 坐标系交互函数
bool checkCoordinateAxisSelection(double mouseX, double mouseY, int windowWidth, int windowHeight,
                                  const glm::mat4& view, const glm::mat4& projection, float cameraDistance);
void rotateSceneAroundAxis(float deltaX, float deltaY, glm::vec2& cameraAngles);
float getCoordinateAxisLength(float cameraDistance);

// 坐标系选择相关的全局变量
extern bool g_isCoordSystemActive;
extern int g_selectedAxis;

#endif // COORDINATE_SYSTEM_H
