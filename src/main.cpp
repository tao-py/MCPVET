#include "mcnp_app.h"
#include "io/scene_manager.h"  // 添加对场景管理器的包含以访问模型文件常量
#include "path/savepath.h"     // 可执行文件路径获取
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
#include <ctime>
#include <filesystem>
#include <chrono>
#include <thread>
#include <memory>

// 新UI架构头文件
#include "ui/MWindows.h"
#include "ui/panels/TopBarWindow.h"
#include "ui/panels/SideBarWindow.h"
#include "ui/panels/BottomBarWindow.h"
#include "ui/panels/ViewportWindow.h"
#include "ui/UILayoutManager.h"
#include "ui/input_control.h"
#include "render/Framebuffer.h"

// 全局变量定义 - 现在从config_manager.h获取

// FPS控制相关 - 现在从config_manager.h获取

static void RenderSceneToViewport(int viewportW, int viewportH)
{
    // 这里替代原来主循环中"渲染网格/对象/坐标系"的那段
    // 注意投影矩阵要用 viewportW/viewportH 的宽高比
    sceneState.projectionMatrix = glm::perspective(
        glm::radians(45.0f),
        (float)viewportW / (float)viewportH,
        0.1f, 100.0f
    );

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(sceneState.viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(sceneState.projectionMatrix));
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(sceneState.cameraPosition));

    if (sceneState.showGrid) {
        renderGrid(sceneState.cameraDistance, sceneState.gridColor);
    }
    for (size_t i = 0; i < meshes.size(); i++) {
        renderMesh(meshes[i]);
    }
    renderCoordinateSystem(sceneState.viewMatrix, sceneState.projectionMatrix, sceneState.cameraDistance);
}

// 函数声明
bool initGLFWAndWindow();
bool initGLAD();
void initOpenGLSettings();
bool initConfig(bool& vsyncEnabled);
bool initLogging();
bool initImGui();
bool initUI(std::unique_ptr<mcnp::ui::UILayoutManager>& layoutManager, bool& vsyncEnabled);
bool initRendering();
bool initDefaultModels();
void mainLoop(bool vsyncEnabled, std::unique_ptr<mcnp::ui::UILayoutManager>& layoutManager);
void cleanup();

int main()
{
    bool vsyncEnabled = false;
    std::unique_ptr<mcnp::ui::UILayoutManager> layoutManager;
    
    // 初始化GLFW和窗口
    if (!initGLFWAndWindow()) {
        return -1;
    }
    
    // 初始化GLAD
    if (!initGLAD()) {
        glfwTerminate();
        return -1;
    }
    
    // 设置OpenGL基本状态
    initOpenGLSettings();
    
    // 加载配置和设置垂直同步
    if (!initConfig(vsyncEnabled)) {
        glfwTerminate();
        return -1;
    }
    
    // 初始化日志系统
    if (!initLogging()) {
        glfwTerminate();
        return -1;
    }
    
    // 初始化ImGui
    if (!initImGui()) {
        glfwTerminate();
        return -1;
    }
    
    // 初始化UI组件
    if (!initUI(layoutManager, vsyncEnabled)) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwTerminate();
        return -1;
    }
    
    // 初始化渲染资源（着色器、网格、坐标系）
    if (!initRendering()) {
        cleanup();
        glfwTerminate();
        return -1;
    }
    
    // 创建默认模型并添加到场景
    if (!initDefaultModels()) {
        cleanup();
        glfwTerminate();
        return -1;
    }
    
    std::cout << "Entering main render loop..." << std::endl;
    std::cout << "Total objects in scene: " << meshes.size() << std::endl;
    
    // 主渲染循环
    mainLoop(vsyncEnabled, layoutManager);
    
    // 保存配置
    saveConfig();
    
    // 刷新并保存日志
    LogManager::getInstance()->flush();
    
    // 清理资源
    cleanup();
    
    glfwTerminate();
    return 0;
}

// 函数定义
bool initGLFWAndWindow()
{
    std::cout << "Initializing GLFW..." << std::endl;
    glfwInit();
    
    // 初始化时间
    lastFrameTime = std::chrono::steady_clock::now();
    deltaTime = 0.0f;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    std::cout << "Setting window hints..." << std::endl;
    
    // 创建窗口
    std::cout << "Creating window..." << std::endl;
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Modeling Tool", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        return false;
    }
    std::cout << "Window created successfully" << std::endl;
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    
    return true;
}

bool initGLAD()
{
    std::cout << "Initializing GLAD..." << std::endl;
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    std::cout << "GLAD initialized successfully" << std::endl;
    
    // 显示OpenGL版本和设备信息
    printOpenGLInfo();
    
    return true;
}

void initOpenGLSettings()
{
    // 设置深度测试
    glEnable(GL_DEPTH_TEST);
}

bool initConfig(bool& vsyncEnabled)
{
    // 加载配置
    std::cout << "Loading config..." << std::endl;
    loadConfig();
    
    // 设置垂直同步（优先使用glfwSwapInterval控制帧率）
    // 注意：垂直同步会将帧率限制在显示器刷新率（通常60Hz）
    // 启用自定义FPS限制时禁用垂直同步以避免冲突
    vsyncEnabled = false; // 始终禁用垂直同步，使用自定义FPS限制
    glfwSwapInterval(0);
    std::cout << "Vertical sync disabled (using custom FPS limit)" << std::endl;
    
    return true;
}

bool initLogging()
{
    std::cout << "Initializing logging system..." << std::endl;
    LogManager* logger = LogManager::getInstance();
    logger->logInfo("Application started");
    logger->logOperation("Initialization", "Starting 3D modeling application");
    return true;
}

bool initImGui()
{
    std::cout << "Initializing ImGui..." << std::endl;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // 设置 imgui.ini 保存到可执行文件同目录下
    static std::string imguiIniPath;
    const std::string& exeDir = getExecutableDirectoryString();
    if (!exeDir.empty()) {
        // 使用 std::filesystem::path 确保跨平台路径分隔符
        imguiIniPath = (std::filesystem::path(exeDir) / "imgui.ini").string();
    } else {
        // 回退到相对路径（相对于当前工作目录）
        imguiIniPath = "imgui.ini";
    }
    io.IniFilename = imguiIniPath.c_str();
    std::cout << "ImGui ini filename: " << io.IniFilename << std::endl;
    // Docking：用于实现可拖拽/可缩放的布局
    // Note: ImGuiConfigFlags_DockingEnable not available in this version
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    
    // 配置支持中文的字体
    // 尝试加载中文字体
    // 注意：对于较新的ImGui后端，不应手动调用Build()
    io.Fonts->Clear();
    // 添加默认字体
    ImFontConfig config;
    config.MergeMode = false;
    config.GlyphOffset.y = 0.0f;
    // 添加基本拉丁字符集
    static const ImWchar ranges_basic[] = {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x2000, 0x206F, // General Punctuation
        0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF, // Half-width characters
        0x4e00, 0x9FAF, // CJK Ideograms
        0,
    };
    io.Fonts->AddFontDefault();
    // 如果存在中文字体，进行合并（缺失则安全跳过）
    config.MergeMode = true;
    const char* fontCandidates[] = {
        "fonts/simkai.ttf",
        "init/fonts/simkai.ttf",
        "../init/fonts/simkai.ttf"
    };
    for (const char* fontPath : fontCandidates) {
        if (std::filesystem::exists(fontPath)) {
            io.Fonts->AddFontFromFileTTF(fontPath, 16.0f, &config, ranges_basic);
            break;
        }
    }
    // 不再调用Build()，让ImGui后端自行处理
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __APPLE__
    ImGui_ImplOpenGL3_Init("#version 150");
#else
    ImGui_ImplOpenGL3_Init("#version 330");
#endif
    std::cout << "ImGui initialized successfully" << std::endl;
    
    // 设置UI字体大小
    updateUIFontSize(sceneState.uiFontSize);
    
    return true;
}

bool initUI(std::unique_ptr<mcnp::ui::UILayoutManager>& layoutManager, bool& vsyncEnabled)
{
    (void)vsyncEnabled; // 未使用参数

    // 创建UI组件，确保它们在程序运行期间保持存活
    auto topBar = std::make_unique<mcnp::ui::TopBarWindow>();
    auto sideBar = std::make_unique<mcnp::ui::SideBarWindow>();
    auto bottomBar = std::make_unique<mcnp::ui::BottomBarWindow>();
    auto viewport = std::make_unique<mcnp::ui::ViewportWindow>(&RenderSceneToViewport);
    
    // 设置全局ViewportWindow指针用于输入控制
    g_viewportWindow = viewport.get();
    topBar->SetWindows(sideBar.get(), bottomBar.get(), viewport.get());
    
    // 创建UILayoutManager实例，它会接管UI组件的所有权
    layoutManager = std::make_unique<mcnp::ui::UILayoutManager>(
        std::move(topBar), 
        std::move(sideBar), 
        std::move(bottomBar), 
        std::move(viewport)
    );
    
    return true;
}

bool initRendering()
{
    // 设置着色器
    std::cout << "Setting up shaders..." << std::endl;
    setupShaders();
    
    // 设置网格
    std::cout << "Setting up grid..." << std::endl;
    setupGrid();
    
    // 设置坐标系
    std::cout << "Setting up coordinate system..." << std::endl;
    setupCoordinateSystem();
    
    return true;
}

bool initDefaultModels()
{
    // 创建基本建模文件
    std::cout << "Creating basic models..." << std::endl;
    Mesh cubeMesh("Cube");
    createCubeMesh(cubeMesh);
    saveModelToFile(cubeMesh, CUBE_MODEL_FILE);
    
    Mesh sphereMesh("Sphere");
    createSphereMesh(sphereMesh);
    saveModelToFile(sphereMesh, SPHERE_MODEL_FILE);
    
    Mesh cylinderMesh("Cylinder");
    createCylinderMesh(cylinderMesh);
    saveModelToFile(cylinderMesh, CYLINDER_MODEL_FILE);
    
    Mesh planeMesh("Plane");
    createPlaneMesh(planeMesh);
    saveModelToFile(planeMesh, PLANE_MODEL_FILE);
    
    // 将基础模型添加到场景中以便渲染
    meshes.push_back(cubeMesh);
    meshes.push_back(sphereMesh);
    meshes.push_back(cylinderMesh);
    meshes.push_back(planeMesh);
    
    // 将模型也添加到原始网格集合中
    originalMeshes.push_back(cubeMesh);
    originalMeshes.push_back(sphereMesh);
    originalMeshes.push_back(cylinderMesh);
    originalMeshes.push_back(planeMesh);
    
    // 设置默认选中第一个对象
    if (!meshes.empty()) {
        selectedMesh = 0;
        meshes[0].selected = true;
    }
    
    return true;
}

void mainLoop(bool vsyncEnabled, std::unique_ptr<mcnp::ui::UILayoutManager>& layoutManager)
{
    // 主渲染循环
    while (!glfwWindowShouldClose(window))
    {
        // 输出每秒的帧数信息（每100帧输出一次，输出前10次）
        static int frameCount = 0;
        frameCount++;
        if (frameCount % 100 == 0 && frameCount <= 1000) {
            std::cout << "Frames rendered: " << frameCount << std::endl;
        }
        
        // 计算deltaTime并实现FPS限制
        auto currentFrameTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> frameDuration = currentFrameTime - lastFrameTime;
        deltaTime = frameDuration.count();
        
        // FPS限制：如果垂直同步禁用且fpsLimit>0，使用sleep进行限制
        if (!vsyncEnabled && sceneState.fpsLimit > 0) {
            float minFrameTime = 1.0f / sceneState.fpsLimit;
            if (deltaTime < minFrameTime) {
                float sleepTime = minFrameTime - deltaTime;
                // 确保sleepTime为正值，防止异常情况
                if (sleepTime > 0) {
                    std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));
                }
                
                // 重新计算deltaTime以反映睡眠后的实际帧时间
                currentFrameTime = std::chrono::steady_clock::now();
                frameDuration = currentFrameTime - lastFrameTime;
                deltaTime = frameDuration.count();
            }
        }
        
        lastFrameTime = currentFrameTime;
        
        processInput(window);
        
        // 开始ImGUI帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // 使用新的UI布局管理器
        layoutManager->DrawAll();
        
        // 为 ImGui 渲染恢复默认 framebuffer 与 viewport（ViewportWindow 会临时改 glViewport）
        int display_w = 0, display_h = 0;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 渲染ImGUI
        glDisable(GL_DEPTH_TEST);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glEnable(GL_DEPTH_TEST);
        
        // 交换缓冲区和轮询事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void cleanup()
{
    // 清理资源
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    // UI组件由 std::unique_ptr 自动释放

    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);
    glDeleteProgram(shaderProgram);
    
    // 释放所有网格资源
    for (auto& mesh : meshes) {
        releaseMeshResources(mesh);
    }
    for (auto& mesh : originalMeshes) {
        releaseMeshResources(mesh);
    }
}
