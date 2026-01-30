#include "config_manager.h"
#include "log_manager.h"
#include "vertex_mesh.h"
#include <vector>
#include <string>

// 全局变量定义
SceneState sceneState;
KeyBindings keyBindings;

// 全局网格变量定义
std::vector<Mesh> meshes;
std::vector<Mesh> originalMeshes;
int selectedMesh = -1;
int secondMeshForBoolean = -1;

// 全局窗口变量定义
GLFWwindow* window;
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// FPS控制相关
std::chrono::steady_clock::time_point lastFrameTime;
float deltaTime = 0.0f;

// 新增：配置文件路径
const std::string CONFIG_FILE = "config.json";

// 从INI文件加载快捷键配置
void loadShortcutsFromIni(const std::string& iniFile)
{
    std::ifstream file(iniFile);
    if (!file.is_open()) {
        LogManager::getInstance()->logWarning("Could not open shortcut configuration file: " + iniFile);
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        // 跳过注释和空行
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }

        // 查找等号分隔符
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            // 去除前后空白字符
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);

            // 根据键名设置相应的快捷键
            if (key == "rotate_camera") {
                keyBindings.rotateCamera = value;
            } else if (key == "pan_camera") {
                keyBindings.panCamera = value;
            } else if (key == "create_cube") {
                keyBindings.createCube = value;
            } else if (key == "create_sphere") {
                keyBindings.createSphere = value;
            } else if (key == "create_cylinder") {
                keyBindings.createCylinder = value;
            } else if (key == "create_plane") {
                keyBindings.createPlane = value;
            } else if (key == "create_line") {
                keyBindings.createLine = value;
            } else if (key == "create_point") {
                keyBindings.createPoint = value;
            } else if (key == "boolean_union") {
                keyBindings.booleanUnion = value;
            } else if (key == "boolean_difference") {
                keyBindings.booleanDifference = value;
            } else if (key == "boolean_intersection") {
                keyBindings.booleanIntersection = value;
            } else if (key == "save_scene") {
                keyBindings.saveScene = value;
            } else if (key == "load_scene") {
                keyBindings.loadScene = value;
            } else if (key == "exit_app") {
                keyBindings.exitApp = value;
            } else if (key == "delete_object") {
                // 如果有删除对象的快捷键，可以在这里添加
            } else if (key == "duplicate_object") {
                // 如果有复制对象的快捷键，可以在这里添加
            } else if (key == "select_all") {
                // 如果有全选的快捷键，可以在这里添加
            }
        }
    }
    file.close();
    
    LogManager::getInstance()->logInfo("Shortcut configuration loaded from " + iniFile);
}

// 保存配置
void saveConfig()
{
    json config;
    
    // 保存快捷键
    config["key_bindings"]["rotate_camera"] = keyBindings.rotateCamera;
    config["key_bindings"]["pan_camera"] = keyBindings.panCamera;
    config["key_bindings"]["create_cube"] = keyBindings.createCube;
    config["key_bindings"]["create_sphere"] = keyBindings.createSphere;
    config["key_bindings"]["create_cylinder"] = keyBindings.createCylinder;
    config["key_bindings"]["create_plane"] = keyBindings.createPlane;
    config["key_bindings"]["create_line"] = keyBindings.createLine;
    config["key_bindings"]["create_point"] = keyBindings.createPoint;
    config["key_bindings"]["boolean_union"] = keyBindings.booleanUnion;
    config["key_bindings"]["boolean_difference"] = keyBindings.booleanDifference;
    config["key_bindings"]["boolean_intersection"] = keyBindings.booleanIntersection;
    config["key_bindings"]["save_scene"] = keyBindings.saveScene;
    config["key_bindings"]["load_scene"] = keyBindings.loadScene;
    config["key_bindings"]["exit_app"] = keyBindings.exitApp;
    
    // 保存UI设置
    config["ui_settings"]["font_size"] = sceneState.uiFontSize;
    config["ui_settings"]["grid_color"] = {sceneState.gridColor.r, sceneState.gridColor.g, sceneState.gridColor.b};
    config["ui_settings"]["show_grid"] = sceneState.showGrid;
    
    // 写入文件（带错误处理）
    std::ofstream file(CONFIG_FILE);
    if (!file.is_open()) {
        LogManager::getInstance()->logError("Failed to open config file for writing: " + CONFIG_FILE);
        return;
    }
    try {
        file << config.dump(4);
        file.close();
        LogManager::getInstance()->logInfo("Configuration saved successfully");
    } catch (const std::exception& e) {
        LogManager::getInstance()->logError("Failed to write config file: " + std::string(e.what()));
        file.close();
    }
}

// 加载配置
void loadConfig()
{
    std::ifstream file(CONFIG_FILE);
    if (!file.good()) {
        // 如果配置文件不存在，使用默认设置
        LogManager::getInstance()->logInfo("Config file not found, using defaults");
        // 尝试从INI文件加载快捷键配置
        loadShortcutsFromIni("inits/shortcut.ini");
        return;
    }
    
    json config;
    try {
        file >> config;
    } catch (const std::exception& e) {
        LogManager::getInstance()->logError("Failed to parse config JSON: " + std::string(e.what()));
        file.close();
        loadShortcutsFromIni("inits/shortcut.ini");
        return;
    }
    file.close();
    
    // 加载快捷键（带类型检查）
    if (config.contains("key_bindings") && config["key_bindings"].is_object()) {
        const auto& kb = config["key_bindings"];
        auto safeAssign = [&](std::string& target, const std::string& key) {
            if (kb.contains(key) && kb[key].is_string()) {
                target = kb[key];
            }
        };
        safeAssign(keyBindings.rotateCamera, "rotate_camera");
        safeAssign(keyBindings.panCamera, "pan_camera");
        safeAssign(keyBindings.createCube, "create_cube");
        safeAssign(keyBindings.createSphere, "create_sphere");
        safeAssign(keyBindings.createCylinder, "create_cylinder");
        safeAssign(keyBindings.createPlane, "create_plane");
        safeAssign(keyBindings.createLine, "create_line");
        safeAssign(keyBindings.createPoint, "create_point");
        safeAssign(keyBindings.booleanUnion, "boolean_union");
        safeAssign(keyBindings.booleanDifference, "boolean_difference");
        safeAssign(keyBindings.booleanIntersection, "boolean_intersection");
        safeAssign(keyBindings.saveScene, "save_scene");
        safeAssign(keyBindings.loadScene, "load_scene");
        safeAssign(keyBindings.exitApp, "exit_app");
    }
    
    // 加载UI设置（带类型检查）
    if (config.contains("ui_settings") && config["ui_settings"].is_object()) {
        const auto& ui = config["ui_settings"];
        if (ui.contains("font_size") && ui["font_size"].is_number()) {
            sceneState.uiFontSize = ui["font_size"];
        }
        if (ui.contains("grid_color") && ui["grid_color"].is_array() && ui["grid_color"].size() >= 3) {
            if (ui["grid_color"][0].is_number()) sceneState.gridColor.r = ui["grid_color"][0];
            if (ui["grid_color"][1].is_number()) sceneState.gridColor.g = ui["grid_color"][1];
            if (ui["grid_color"][2].is_number()) sceneState.gridColor.b = ui["grid_color"][2];
        }
        if (ui.contains("show_grid") && ui["show_grid"].is_boolean()) {
            sceneState.showGrid = ui["show_grid"];
        }
    }
    
    // 从INI文件加载快捷键配置（优先级更高）
    loadShortcutsFromIni("inits/shortcut.ini");
    
    LogManager::getInstance()->logInfo("Configuration loaded successfully");
}

void updateUIFontSize(float size)
{
    sceneState.uiFontSize = size;
    // 安全地更新ImGui字体大小，确保上下文存在
    if (ImGui::GetCurrentContext() != nullptr) {
        ImGui::GetIO().FontGlobalScale = size / 16.0f;
    }
}

bool checkKeyBinding(const std::string& binding, int key, int mods)
{
    if (binding == "MOUSE_MIDDLE") return false; // 鼠标按键在鼠标回调中处理
    if (binding == "MOUSE_RIGHT") return false;
    
    // 解析组合键
    bool ctrl = binding.find("CTRL+") != std::string::npos;
    bool shift = binding.find("SHIFT+") != std::string::npos;
    bool alt = binding.find("ALT+") != std::string::npos;
    
    // 提取主键
    std::string mainKey = binding;
    if (ctrl) mainKey = mainKey.substr(5);
    if (shift) mainKey = mainKey.substr(6);
    if (alt) mainKey = mainKey.substr(4);
    
    // 检查修饰键
    bool modsMatch = true;
    if (ctrl) modsMatch &= (mods & GLFW_MOD_CONTROL) != 0;
    if (shift) modsMatch &= (mods & GLFW_MOD_SHIFT) != 0;
    if (alt) modsMatch &= (mods & GLFW_MOD_ALT) != 0;
    
    // 检查主键
    int targetKey = stringToKey(mainKey);
    if (targetKey == -1) return false;
    
    return modsMatch && (key == targetKey);
}

std::string keyToString(int key)
{
    switch (key) {
        case GLFW_KEY_A: return "A";
        case GLFW_KEY_B: return "B";
        case GLFW_KEY_C: return "C";
        case GLFW_KEY_D: return "D";
        case GLFW_KEY_E: return "E";
        case GLFW_KEY_F: return "F";
        case GLFW_KEY_G: return "G";
        case GLFW_KEY_H: return "H";
        case GLFW_KEY_I: return "I";
        case GLFW_KEY_J: return "J";
        case GLFW_KEY_K: return "K";
        case GLFW_KEY_L: return "L";
        case GLFW_KEY_M: return "M";
        case GLFW_KEY_N: return "N";
        case GLFW_KEY_O: return "O";
        case GLFW_KEY_P: return "P";
        case GLFW_KEY_Q: return "Q";
        case GLFW_KEY_R: return "R";
        case GLFW_KEY_S: return "S";
        case GLFW_KEY_T: return "T";
        case GLFW_KEY_U: return "U";
        case GLFW_KEY_V: return "V";
        case GLFW_KEY_W: return "W";
        case GLFW_KEY_X: return "X";
        case GLFW_KEY_Y: return "Y";
        case GLFW_KEY_Z: return "Z";
        case GLFW_KEY_0: return "0";
        case GLFW_KEY_1: return "1";
        case GLFW_KEY_2: return "2";
        case GLFW_KEY_3: return "3";
        case GLFW_KEY_4: return "4";
        case GLFW_KEY_5: return "5";
        case GLFW_KEY_6: return "6";
        case GLFW_KEY_7: return "7";
        case GLFW_KEY_8: return "8";
        case GLFW_KEY_9: return "9";
        case GLFW_KEY_ESCAPE: return "ESCAPE";
        case GLFW_KEY_ENTER: return "ENTER";
        case GLFW_KEY_SPACE: return "SPACE";
        default: return "UNKNOWN";
    }
}

int stringToKey(const std::string& str)
{
    static std::map<std::string, int> keyMap = {
        {"A", GLFW_KEY_A},
        {"B", GLFW_KEY_B},
        {"C", GLFW_KEY_C},
        {"D", GLFW_KEY_D},
        {"E", GLFW_KEY_E},
        {"F", GLFW_KEY_F},
        {"G", GLFW_KEY_G},
        {"H", GLFW_KEY_H},
        {"I", GLFW_KEY_I},
        {"J", GLFW_KEY_J},
        {"K", GLFW_KEY_K},
        {"L", GLFW_KEY_L},
        {"M", GLFW_KEY_M},
        {"N", GLFW_KEY_N},
        {"O", GLFW_KEY_O},
        {"P", GLFW_KEY_P},
        {"Q", GLFW_KEY_Q},
        {"R", GLFW_KEY_R},
        {"S", GLFW_KEY_S},
        {"T", GLFW_KEY_T},
        {"U", GLFW_KEY_U},
        {"V", GLFW_KEY_V},
        {"W", GLFW_KEY_W},
        {"X", GLFW_KEY_X},
        {"Y", GLFW_KEY_Y},
        {"Z", GLFW_KEY_Z},
        {"0", GLFW_KEY_0},
        {"1", GLFW_KEY_1},
        {"2", GLFW_KEY_2},
        {"3", GLFW_KEY_3},
        {"4", GLFW_KEY_4},
        {"5", GLFW_KEY_5},
        {"6", GLFW_KEY_6},
        {"7", GLFW_KEY_7},
        {"8", GLFW_KEY_8},
        {"9", GLFW_KEY_9},
        {"ESCAPE", GLFW_KEY_ESCAPE},
        {"ENTER", GLFW_KEY_ENTER},
        {"SPACE", GLFW_KEY_SPACE}
    };
    
    if (keyMap.find(str) != keyMap.end()) {
        return keyMap[str];
    }
    return -1;
}
