#ifndef APP_STATE_H
#define APP_STATE_H

#include "vertex_mesh.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <chrono>

namespace mcnp::core {

// 场景状态结构（从 config_manager.h 迁移）
struct SceneState {
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::vec3 cameraPosition;
    glm::vec3 cameraTarget;
    float cameraDistance;
    glm::vec2 cameraAngles;
    
    glm::vec3 gridColor;
    bool showGrid;
    
    // UI字体大小
    float uiFontSize;
    
    // FPS限制
    int fpsLimit;
    
    SceneState() : 
        viewMatrix(glm::mat4(1.0f)),
        projectionMatrix(glm::mat4(1.0f)),
        cameraPosition(0.0f, 0.0f, 5.0f),
        cameraTarget(0.0f, 0.0f, 0.0f),
        cameraDistance(5.0f),
        cameraAngles(0.0f, 0.0f),
        gridColor(0.5f, 0.5f, 0.5f),
        showGrid(true),
        uiFontSize(30.0f),
        fpsLimit(60) {}
};

// 快捷键配置（从 config_manager.h 迁移）
struct KeyBindings {
    std::string rotateCamera = "MOUSE_RIGHT";
    std::string panCamera = "MOUSE_MIDDLE";
    std::string createCube = "K";
    std::string createSphere = "L";
    std::string createCylinder = "M";
    std::string createPlane = "N";
    std::string createLine = "O";
    std::string createPoint = "P";
    std::string booleanUnion = "U";
    std::string booleanDifference = "D";
    std::string booleanIntersection = "I";
    std::string saveScene = "CTRL+S";
    std::string loadScene = "CTRL+O";
    std::string exitApp = "ESCAPE";
};

// 应用程序全局状态
class AppState {
public:
    // 场景状态
    SceneState sceneState;
    KeyBindings keyBindings;
    
    // 网格数据
    std::vector<Mesh> meshes;               // 当前场景中的网格（拥有OpenGL资源）
    std::vector<Mesh> originalMeshes;       // 原始网格数据（不持有OpenGL资源，仅用于布尔运算）
    int selectedMesh = -1;
    int secondMeshForBoolean = -1;
    
    // 窗口与渲染
    GLFWwindow* window = nullptr;
    const unsigned int SCR_WIDTH = 1280;
    const unsigned int SCR_HEIGHT = 720;
    
    // FPS控制
    std::chrono::steady_clock::time_point lastFrameTime;
    float deltaTime = 0.0f;
    
    // 坐标系选择
    bool g_isCoordSystemActive = false;
    int g_selectedAxis = -1;
    
    // 渲染资源（将由RAII类管理，暂时保留裸句柄）
    unsigned int shaderProgram = 0;
    unsigned int gridVAO = 0, gridVBO = 0;
    unsigned int coordVAO = 0, coordVBO = 0;
    
    // 配置路径
    const std::string CONFIG_FILE = "config.json";
    
    AppState() = default;
    
    // 禁止拷贝（单例或通过引用传递）
    AppState(const AppState&) = delete;
    AppState& operator=(const AppState&) = delete;
    
    // 允许移动
    AppState(AppState&&) = default;
    AppState& operator=(AppState&&) = default;
    
    // 资源清理
    void cleanup();
    
    // 确保originalMeshes不持有OpenGL资源
    void clearOriginalMeshesGLResources();
    
    // 获取单例实例（可选，目前使用依赖注入）
    static AppState& getInstance();
};

} // namespace mcnp::core

#endif // APP_STATE_H