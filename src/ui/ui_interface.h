#ifndef UI_INTERFACE_H
#define UI_INTERFACE_H

#include "vertex_mesh.h"
#include "render.h"
#include "config_manager.h"
#include "scene_manager.h"  // 包含scene_manager以获得performBooleanOperation声明
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 全局变量声明 - 现在从config_manager.h获取
// extern GLFWwindow* window;
// extern const unsigned int SCR_WIDTH;
// extern const unsigned int SCR_HEIGHT;

// extern SceneState sceneState;
// extern std::vector<Mesh> meshes;
// extern std::vector<Mesh> originalMeshes;
// extern int selectedMesh;
// extern int secondMeshForBoolean;

// extern KeyBindings keyBindings;

// 布尔运算函数声明
void performBooleanOperation(BooleanOperation operation);

// UI界面相关函数
void renderUI();

#endif // UI_INTERFACE_H