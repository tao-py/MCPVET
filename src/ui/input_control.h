#ifndef INPUT_CONTROL_H
#define INPUT_CONTROL_H

#include "vertex_mesh.h"
#include "render.h"
#include "ui_interface.h"
#include "config_manager.h"
#include "scene_manager.h"
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

// 前向声明
namespace mcnp::ui {
    class ViewportWindow;
}

// 全局ViewportWindow指针
extern mcnp::ui::ViewportWindow* g_viewportWindow;

// 物体选择函数
int pickObject(double mouseX, double mouseY, int windowWidth, int windowHeight);

// 输入控制相关函数
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif // INPUT_CONTROL_H