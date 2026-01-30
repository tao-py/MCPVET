#include "input_control.h"
#include "transform_controller.h"
#include "coordinate_system.h"  // 为了访问坐标系交互函数
#include "../ui/panels/ViewportWindow.h"
#include <limits>
#include <chrono>
#include <imgui.h>
#include <iostream>

// Viewport悬停状态的外部引用
mcnp::ui::ViewportWindow* g_viewportWindow = nullptr;

// 检查鼠标是否在Viewport上
bool isMouseInViewport(GLFWwindow* window)
{
    // 检查ImGui上下文是否存在
    if (!ImGui::GetCurrentContext()) {
        // ImGui未初始化，认为在Viewport上
        return true;
    }
    
    // 严格检查：只有当鼠标确实在ViewportWindow上时才返回true
    // 移除回退逻辑，避免在操作侧边栏时误判
    bool hovered = g_viewportWindow && g_viewportWindow->IsHovered();
    static bool lastHovered = false;
    if (hovered != lastHovered) {
        lastHovered = hovered;
        // Log hover state change for debugging
        // LogManager::getInstance()->logOperation("Input", std::string("Viewport hover: ") + (hovered ? "true" : "false"));
    }
    return hovered;
}

// 全局变量定义（已在coordinate_system.cpp中定义，这里不需要重复定义）
// bool g_isCoordSystemActive = false;
// int g_selectedAxis = -1; // -1: none, 0: X, 1: Y, 2: Z
// 全局变换控制器
TransformController transformCtrl;

// 滚动速度控制
static std::chrono::steady_clock::time_point lastScrollTime = std::chrono::steady_clock::now();
static double accumulatedScroll = 0.0;

// 处理输入
void processInput(GLFWwindow* window)
{
    // 处理键盘快捷键
    // 这部分在key_callback中处理
}

// 窗口大小改变回调
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    sceneState.projectionMatrix = glm::perspective(
        glm::radians(45.0f), 
        (float)width / (float)height, 
        0.1f, 100.0f
    );
}

// 鼠标移动回调
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    static double lastX = xpos;
    static double lastY = ypos;
    static bool firstMouse = true;
    
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    // 检查鼠标是否在Viewport上
    bool inViewport = isMouseInViewport(window);
    static bool lastInViewportButton = false;
    if (inViewport != lastInViewportButton) {
        lastInViewportButton = inViewport;
        std::cout << "[Mouse Button] inViewport: " << (inViewport ? "true" : "false") << std::endl;
    }
    static bool lastInViewport = false;
    if (inViewport != lastInViewport) {
        lastInViewport = inViewport;
        std::cout << "[Mouse Button] inViewport: " << (inViewport ? "true" : "false") << std::endl;
    }
    static bool lastInViewportMove = false;
    if (inViewport != lastInViewportMove) {
        lastInViewportMove = inViewport;
        std::cout << "[Mouse] inViewport: " << (inViewport ? "true" : "false") << std::endl;
    }
    
    // 检查是否正在进行坐标系轴旋转
    if (g_isCoordSystemActive && g_selectedAxis != -1 && inViewport) {
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        
        // 旋转整个场景
        rotateSceneAroundAxis(xoffset, yoffset, sceneState.cameraAngles);
    }
    // 检查是否正在进行对象变换
    else if (transformCtrl.isTransforming() && selectedMesh >= 0 && selectedMesh < meshes.size() && inViewport) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            // 左键拖拽移动对象
            transformCtrl.updateDrag(xpos, ypos, width, height, meshes[selectedMesh]);
        } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            // 右键缩放对象
            transformCtrl.updateScale(xpos, ypos, meshes[selectedMesh]);
        } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
            // 中键旋转对象
            transformCtrl.updateRotate(xpos, ypos, meshes[selectedMesh]);
        }
    }
    // 否则执行相机控制
    else {
        // 中键旋转相机
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS && inViewport) {
            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos;
            
            sceneState.cameraAngles.x += xoffset * 0.01f;
            sceneState.cameraAngles.y += yoffset * 0.01f;
            
            // 限制角度范围
            if (sceneState.cameraAngles.y > glm::pi<float>() / 2.0f - 0.1f)
                sceneState.cameraAngles.y = glm::pi<float>() / 2.0f - 0.1f;
            if (sceneState.cameraAngles.y < -glm::pi<float>() / 2.0f + 0.1f)
                sceneState.cameraAngles.y = -glm::pi<float>() / 2.0f + 0.1f;
        }
        
        // 右键平移相机
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && inViewport) {
            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos;
            
            // 计算平移向量
            glm::vec3 forward = glm::normalize(sceneState.cameraTarget - sceneState.cameraPosition);
            glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
            glm::vec3 up = glm::normalize(glm::cross(right, forward));
            
            // 根据距离调整平移速度
            float panSpeed = sceneState.cameraDistance * 0.001f;
            
            sceneState.cameraTarget += right * xoffset * panSpeed;
            sceneState.cameraTarget += up * yoffset * panSpeed;
        }
    }
    
    lastX = xpos;
    lastY = ypos;
}

// 鼠标滚轮回调
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // 检查鼠标是否在Viewport上
    if (!isMouseInViewport(window)) return;
    
    // 计算时间差，用于计算滚动速度
    auto currentTime = std::chrono::steady_clock::now();
    auto timeDelta = std::chrono::duration<double, std::milli>(currentTime - lastScrollTime).count();
    
    // 累积滚动值
    accumulatedScroll += yoffset;
    
    // 根据滚动速度调整缩放敏感度
    double speedFactor = 1.0;
    if (timeDelta < 100) {  // 如果上次滚动在100ms内
        // 如果快速连续滚动，增加速度因子
        speedFactor = 1.0 + (100.0 - timeDelta) / 100.0;  // 最大2倍速度
    }
    
    // 根据累积的滚动量和速度因子调整距离
    double zoomAmount = accumulatedScroll * 0.2f * speedFactor;
    sceneState.cameraDistance -= zoomAmount;
    
    // 重置累积滚动值
    accumulatedScroll = 0.0;
    
    // 更新最后滚动时间
    lastScrollTime = currentTime;
    
    // 限制距离范围
    if (sceneState.cameraDistance < 1.0f) sceneState.cameraDistance = 1.0f;
    if (sceneState.cameraDistance > 50.0f) sceneState.cameraDistance = 50.0f;
}

// 鼠标按键回调
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // 检查鼠标是否在Viewport上
    bool inViewport = isMouseInViewport(window);
    
    // 左键选择对象或开始拖拽
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        if (action == GLFW_PRESS) {
            // 获取窗口尺寸
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            
            // 检查是否点击了坐标系
            if (inViewport && checkCoordinateAxisSelection(xpos, ypos, width, height,
                                            sceneState.viewMatrix, sceneState.projectionMatrix,
                                            sceneState.cameraDistance)) {
                // 已选择坐标系轴，准备旋转场景
                return;
            }
            
            // 使用射线投射进行精确对象选择
            int pickedObject = inViewport ? pickObject(xpos, height - ypos, width, height) : -1;
            
            // 更新选择状态
            for (int i = 0; i < meshes.size(); i++) {
                meshes[i].selected = false;
            }
            
            selectedMesh = pickedObject;
            
            if (selectedMesh >= 0) {
                meshes[selectedMesh].selected = true;
                // 开始拖拽操作
                transformCtrl.startDrag(xpos, ypos, meshes[selectedMesh]);
            }
        } else if (action == GLFW_RELEASE) {
            // 释放鼠标时取消坐标系选择状态
            g_isCoordSystemActive = false;
            g_selectedAxis = -1;
            
            // 结束拖拽操作
            transformCtrl.endDrag();
        }
    }
    
    // 右键用于缩放
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        if (action == GLFW_PRESS && selectedMesh >= 0 && inViewport) {
            transformCtrl.startScale(xpos, ypos, meshes[selectedMesh]);
        } else if (action == GLFW_RELEASE) {
            // 释放鼠标时取消坐标系选择状态
            g_isCoordSystemActive = false;
            g_selectedAxis = -1;
            
            transformCtrl.endScale();
        }
    }
    
    // 中键用于旋转
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        if (action == GLFW_PRESS && selectedMesh >= 0 && inViewport) {
            transformCtrl.startRotate(xpos, ypos, meshes[selectedMesh]);
        } else if (action == GLFW_RELEASE) {
            // 释放鼠标时取消坐标系选择状态
            g_isCoordSystemActive = false;
            g_selectedAxis = -1;
            
            transformCtrl.endRotate();
        }
    }
}

// 键盘按键回调
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        // 检查快捷键
        if (checkKeyBinding(keyBindings.createCube, key, mods)) {
            Mesh newMesh;
            if (loadModelFromFile(newMesh, CUBE_MODEL_FILE)) {
                newMesh.name = "Cube_" + std::to_string(meshes.size() + 1);
                meshes.push_back(newMesh);
                originalMeshes.push_back(newMesh);
                selectedMesh = meshes.size() - 1;
            }
        }
        else if (checkKeyBinding(keyBindings.createSphere, key, mods)) {
            Mesh newMesh;
            if (loadModelFromFile(newMesh, SPHERE_MODEL_FILE)) {
                newMesh.name = "Sphere_" + std::to_string(meshes.size() + 1);
                meshes.push_back(newMesh);
                originalMeshes.push_back(newMesh);
                selectedMesh = meshes.size() - 1;
            }
        }
        else if (checkKeyBinding(keyBindings.createCylinder, key, mods)) {
            Mesh newMesh;
            if (loadModelFromFile(newMesh, CYLINDER_MODEL_FILE)) {
                newMesh.name = "Cylinder_" + std::to_string(meshes.size() + 1);
                meshes.push_back(newMesh);
                originalMeshes.push_back(newMesh);
                selectedMesh = meshes.size() - 1;
            }
        }
        else if (checkKeyBinding(keyBindings.createPlane, key, mods)) {
            Mesh newMesh;
            if (loadModelFromFile(newMesh, PLANE_MODEL_FILE)) {
                newMesh.name = "Plane_" + std::to_string(meshes.size() + 1);
                meshes.push_back(newMesh);
                originalMeshes.push_back(newMesh);
                selectedMesh = meshes.size() - 1;
            }
        }
        else if (checkKeyBinding(keyBindings.createLine, key, mods)) {
            Mesh newMesh("Line_" + std::to_string(meshes.size() + 1));
            createLineMesh(newMesh);
            meshes.push_back(newMesh);
            originalMeshes.push_back(newMesh);
            selectedMesh = meshes.size() - 1;
        }
        else if (checkKeyBinding(keyBindings.createPoint, key, mods)) {
            Mesh newMesh("Point_" + std::to_string(meshes.size() + 1));
            createPointMesh(newMesh);
            meshes.push_back(newMesh);
            originalMeshes.push_back(newMesh);
            selectedMesh = meshes.size() - 1;
        }
        else if (checkKeyBinding(keyBindings.booleanUnion, key, mods)) {
            performBooleanOperation(BooleanOperation::UNION);
        }
        else if (checkKeyBinding(keyBindings.booleanDifference, key, mods)) {
            performBooleanOperation(BooleanOperation::DIFFERENCE);
        }
        else if (checkKeyBinding(keyBindings.booleanIntersection, key, mods)) {
            performBooleanOperation(BooleanOperation::INTERSECTION);
        }
        else if (checkKeyBinding(keyBindings.saveScene, key, mods)) {
            saveScene("user/scenes/scene.json");
        }
        else if (checkKeyBinding(keyBindings.loadScene, key, mods)) {
            loadScene("user/scenes/scene.json");
        }
        else if (checkKeyBinding(keyBindings.exitApp, key, mods)) {
            glfwSetWindowShouldClose(window, true);
        }
    }
}
