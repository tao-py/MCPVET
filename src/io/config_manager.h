#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>
#include <fstream>
#include <map>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <nlohmann/json.hpp>
#include <vector>
#include <chrono>
#include "vertex_mesh.h"

using json = nlohmann::json;

// 前向声明以避免循环依赖
struct SceneState;
struct KeyBindings;

// 场景状态结构
struct SceneState {
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::vec3 cameraPosition;
    glm::vec3 cameraTarget;
    float cameraDistance;
    glm::vec2 cameraAngles;
    
    glm::vec3 gridColor;
    bool showGrid;
    
    // 新增：UI字体大小
    float uiFontSize;
    
    // 新增：FPS限制
    int fpsLimit;

    // 新增：UI布局尺寸（非Docking模式持久化）
    float uiSideWidth;
    float uiBottomHeight;
    
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
        fpsLimit(60),
        uiSideWidth(320.0f),
        uiBottomHeight(200.0f) {}
};

// 快捷键配置
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

// 全局变量声明
extern SceneState sceneState;
extern KeyBindings keyBindings;

// 全局网格变量声明
extern std::vector<Mesh> meshes;
extern std::vector<Mesh> originalMeshes;
extern int selectedMesh;
extern int secondMeshForBoolean;

// 在此处添加其他全局变量声明，以避免循环依赖
extern GLFWwindow* window;
extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

// FPS控制相关
extern std::chrono::steady_clock::time_point lastFrameTime;
extern float deltaTime;

// 函数声明
void saveConfig();
void loadConfig();
void updateUIFontSize(float size);
bool checkKeyBinding(const std::string& binding, int key, int mods);
std::string keyToString(int key);
int stringToKey(const std::string& str);

// 从INI文件加载快捷键配置
void loadShortcutsFromIni(const std::string& iniFile);

// 声明坐标系选择相关的全局变量
extern bool g_isCoordSystemActive;
extern int g_selectedAxis;

#endif
