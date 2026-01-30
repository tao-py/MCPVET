#include "ui_interface.h"
#include "scene_manager.h"
#include "log_manager.h"
#include "command_parser.h"
#include "geometry_factory.h"
#include "language_manager.h"
#include "transform_controller.h"
#include "config_manager.h"  // 包含全局变量声明
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "ImGuiFileDialog.h"

// 全局命令解析器实例
static CommandParser commandParser;
static char commandInput[256] = "";

// 全局语言管理器实例
static LanguageManager languageManager("init/languages/en_US.json");

using namespace IGFD;

// 文件对话框相关变量
static FileDialog fileDialog;

// 初始化文件对话框
void initFileDialog() {
    // ImGuiFileDialog不需要特殊初始化
}

// 渲染用户界面
void renderUI()
{
    // 检查ImGui上下文是否存在
    if (!ImGui::GetCurrentContext()) {
        return; // 如果上下文不存在，则跳过UI渲染
    }

#ifdef IMGUI_HAS_DOCK
    // DockSpace host
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
#if defined(IMGUI_HAS_VIEWPORT)
    ImGui::SetNextWindowViewport(viewport->ID);
#endif
    ImGuiWindowFlags dockspaceFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
    dockspaceFlags |= ImGuiWindowFlags_NoDocking;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("DockSpaceWindow", nullptr, dockspaceFlags);
    ImGui::PopStyleVar(2);
    ImGuiID dockspaceId = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    ImGui::End();
#endif

    // 主菜单栏
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New", "Ctrl+N")) {
                meshes.clear();
                originalMeshes.clear();
                selectedMesh = -1;
                secondMeshForBoolean = -1;
                LogManager::getInstance()->logOperation("File Operation", "New scene created");
            }
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                initFileDialog();
                FileDialogConfig config;
                config.path = "user/scenes/";
                
                // 设置文件对话框的默认大小，与主窗口大小保持一定比例
                ImVec2 dialogSize = ImGui::GetMainViewport()->Size;
                dialogSize.x *= 0.7f;  // 主窗口宽度的70%
                dialogSize.y *= 0.6f;  // 主窗口高度的60%
                ImGui::SetNextWindowSize(dialogSize);
                
                fileDialog.OpenDialog("ChooseFileToOpenDlgKey", "Choose Scene File", ".json,.JSON", config);
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                saveScene("user/scenes/scene.json");
                LogManager::getInstance()->logOperation("File Operation", "Scene saved to user/scenes/scene.json");
            }
            if (ImGui::MenuItem("Save As")) {
                initFileDialog();
                FileDialogConfig config;
                config.path = "user/scenes/";
                
                // 设置文件对话框的默认大小，与主窗口大小保持一定比例
                ImVec2 dialogSize = ImGui::GetMainViewport()->Size;
                dialogSize.x *= 0.7f;  // 主窗口宽度的70%
                dialogSize.y *= 0.6f;  // 主窗口高度的60%
                ImGui::SetNextWindowSize(dialogSize);
                
                fileDialog.OpenDialog("ChooseFileDlgKey", "Save Scene As", ".json,.JSON", config);
            }
            if (ImGui::MenuItem("Exit", "Esc")) {
                LogManager::getInstance()->logInfo("Application exit initiated");
                glfwSetWindowShouldClose(window, true);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Create")) {
            if (ImGui::MenuItem("Cube", "C")) {
                Mesh newMesh;
                if (loadModelFromFile(newMesh, CUBE_MODEL_FILE)) {
                    newMesh.name = "Cube_" + std::to_string(meshes.size() + 1);
                    meshes.push_back(newMesh);
                    originalMeshes.push_back(newMesh);
                    selectedMesh = meshes.size() - 1;
                    LogManager::getInstance()->logOperation("Object Creation", "Cube created: " + newMesh.name);
                }
            }
            if (ImGui::MenuItem("Sphere", "S")) {
                Mesh newMesh;
                if (loadModelFromFile(newMesh, SPHERE_MODEL_FILE)) {
                    newMesh.name = "Sphere_" + std::to_string(meshes.size() + 1);
                    meshes.push_back(newMesh);
                    originalMeshes.push_back(newMesh);
                    selectedMesh = meshes.size() - 1;
                    LogManager::getInstance()->logOperation("Object Creation", "Sphere created: " + newMesh.name);
                }
            }
            if (ImGui::MenuItem("Cylinder", "Y")) {
                Mesh newMesh;
                if (loadModelFromFile(newMesh, CYLINDER_MODEL_FILE)) {
                    newMesh.name = "Cylinder_" + std::to_string(meshes.size() + 1);
                    meshes.push_back(newMesh);
                    originalMeshes.push_back(newMesh);
                    selectedMesh = meshes.size() - 1;
                    LogManager::getInstance()->logOperation("Object Creation", "Cylinder created: " + newMesh.name);
                }
            }
            if (ImGui::MenuItem("Plane", "P")) {
                Mesh newMesh;
                if (loadModelFromFile(newMesh, PLANE_MODEL_FILE)) {
                    newMesh.name = "Plane_" + std::to_string(meshes.size() + 1);
                    meshes.push_back(newMesh);
                    originalMeshes.push_back(newMesh);
                    selectedMesh = meshes.size() - 1;
                    LogManager::getInstance()->logOperation("Object Creation", "Plane created: " + newMesh.name);
                }
            }
            if (ImGui::MenuItem("Line", "L")) {
                Mesh newMesh("Line_" + std::to_string(meshes.size() + 1));
                createLineMesh(newMesh);
                meshes.push_back(newMesh);
                originalMeshes.push_back(newMesh);
                selectedMesh = meshes.size() - 1;
                LogManager::getInstance()->logOperation("Object Creation", "Line created: " + newMesh.name);
            }
            if (ImGui::MenuItem("Point", "O")) {
                Mesh newMesh("Point_" + std::to_string(meshes.size() + 1));
                createPointMesh(newMesh);
                meshes.push_back(newMesh);
                originalMeshes.push_back(newMesh);
                selectedMesh = meshes.size() - 1;
                LogManager::getInstance()->logOperation("Object Creation", "Point created: " + newMesh.name);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
                // 撤销操作实现
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
                // 重做操作实现
            }
            if (ImGui::MenuItem("Delete", "Del")) {
                if (selectedMesh >= 0 && selectedMesh < static_cast<int>(meshes.size())) {
                    std::string deletedObjectName = meshes[selectedMesh].name;
                    meshes.erase(meshes.begin() + selectedMesh);
                    originalMeshes.erase(originalMeshes.begin() + selectedMesh);
                    LogManager::getInstance()->logOperation("Object Deletion", "Object deleted: " + deletedObjectName);
                    selectedMesh = -1;
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Boolean")) {
            if (ImGui::MenuItem("Union", "U")) {
                performBooleanOperation(BooleanOperation::UNION);
            }
            if (ImGui::MenuItem("Difference", "D")) {
                performBooleanOperation(BooleanOperation::DIFFERENCE);
            }
            if (ImGui::MenuItem("Intersection", "I")) {
                performBooleanOperation(BooleanOperation::INTERSECTION);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings")) {
            if (ImGui::MenuItem("Key Bindings")) {
                // 打开快捷键设置对话框
            }
            if (ImGui::MenuItem("UI Settings")) {
                // 打开UI设置对话框
            }
            if (ImGui::BeginMenu(languageManager.getText("Language").c_str())) {
                if (ImGui::MenuItem(languageManager.getText("English").c_str())) {
                    languageManager.setLanguage(Language::ENGLISH);
                }
                if (ImGui::MenuItem(languageManager.getText("Chinese").c_str())) {
                    languageManager.setLanguage(Language::CHINESE);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    
    // 侧边工具栏 - 确保在非DockSpace模式下也能显示
    ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(280, ImGui::GetMainViewport()->Size.y - 50), ImGuiCond_FirstUseEver);
    ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_NoCollapse);
    
    // 工具栏选项卡
    ImGui::BeginTabBar("ToolsTabs");
    
    if (ImGui::BeginTabItem("View")) {
        ImGui::Text("View Options");
        ImGui::Checkbox("Show Grid", &sceneState.showGrid);
        ImGui::ColorEdit3("Grid Color", glm::value_ptr(sceneState.gridColor));
        
        ImGui::Separator();
        ImGui::Text("Camera");
        ImGui::Text("Distance: %.2f", sceneState.cameraDistance);
        ImGui::Text("Angle X: %.2f", sceneState.cameraAngles.x);
        ImGui::Text("Angle Y: %.2f", sceneState.cameraAngles.y);
        
        ImGui::Separator();
        ImGui::Text("Performance");
        if (ImGui::SliderInt("FPS Limit", &sceneState.fpsLimit, 30, 120, "%d FPS")) {
            // FPS限制值已更改
        }
        
        ImGui::Separator();
        ImGui::Text("UI Settings");
        if (ImGui::SliderFloat("Font Size", &sceneState.uiFontSize, 12.0f, 24.0f, "%.1f")) {
            updateUIFontSize(sceneState.uiFontSize);
        }
        
        ImGui::EndTabItem();
    }
    
    if (ImGui::BeginTabItem("Objects")) {
        ImGui::Text("Objects (%d)", (int)meshes.size());
        
        // 对象创建按钮
        if (ImGui::Button("Add Cube", ImVec2(-1, 0))) {
            Mesh newMesh;
            if (loadModelFromFile(newMesh, CUBE_MODEL_FILE)) {
                newMesh.name = "Cube_" + std::to_string(meshes.size() + 1);
                meshes.push_back(newMesh);
                originalMeshes.push_back(newMesh);
                selectedMesh = meshes.size() - 1;
            }
        }
        if (ImGui::Button("Add Sphere", ImVec2(-1, 0))) {
            Mesh newMesh;
            if (loadModelFromFile(newMesh, SPHERE_MODEL_FILE)) {
                newMesh.name = "Sphere_" + std::to_string(meshes.size() + 1);
                meshes.push_back(newMesh);
                originalMeshes.push_back(newMesh);
                selectedMesh = meshes.size() - 1;
            }
        }
        if (ImGui::Button("Add Cylinder", ImVec2(-1, 0))) {
            Mesh newMesh;
            if (loadModelFromFile(newMesh, CYLINDER_MODEL_FILE)) {
                newMesh.name = "Cylinder_" + std::to_string(meshes.size() + 1);
                meshes.push_back(newMesh);
                originalMeshes.push_back(newMesh);
                selectedMesh = meshes.size() - 1;
            }
        }
        if (ImGui::Button("Add Plane", ImVec2(-1, 0))) {
            Mesh newMesh;
            if (loadModelFromFile(newMesh, PLANE_MODEL_FILE)) {
                newMesh.name = "Plane_" + std::to_string(meshes.size() + 1);
                meshes.push_back(newMesh);
                originalMeshes.push_back(newMesh);
                selectedMesh = meshes.size() - 1;
            }
        }
        if (ImGui::Button("Add Point", ImVec2(-1, 0))) {
            Mesh newMesh("Point_" + std::to_string(meshes.size() + 1));
            createPointMesh(newMesh);
            meshes.push_back(newMesh);
            originalMeshes.push_back(newMesh);
            selectedMesh = meshes.size() - 1;
            LogManager::getInstance()->logOperation("Object Creation", "Point created: " + newMesh.name);
        }
        if (ImGui::Button("Add Line", ImVec2(-1, 0))) {
            Mesh newMesh("Line_" + std::to_string(meshes.size() + 1));
            createLineMesh(newMesh, glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec3(0.5f, 0.0f, 0.0f));
            meshes.push_back(newMesh);
            originalMeshes.push_back(newMesh);
            selectedMesh = meshes.size() - 1;
            LogManager::getInstance()->logOperation("Object Creation", "Line created: " + newMesh.name);
        }
        if (ImGui::Button("Add Circle", ImVec2(-1, 0))) {
            Mesh newMesh("Circle_" + std::to_string(meshes.size() + 1));
            createCircleMesh(newMesh);
            meshes.push_back(newMesh);
            originalMeshes.push_back(newMesh);
            selectedMesh = meshes.size() - 1;
            LogManager::getInstance()->logOperation("Object Creation", "Circle created: " + newMesh.name);
        }
        if (ImGui::Button("Add Sphere", ImVec2(-1, 0))) {
            Mesh newMesh("Sphere_" + std::to_string(meshes.size() + 1));
            GeometryFactory::createSphere(newMesh);
            meshes.push_back(newMesh);
            originalMeshes.push_back(newMesh);
            selectedMesh = meshes.size() - 1;
            LogManager::getInstance()->logOperation("Object Creation", "Sphere created: " + newMesh.name);
        }
        if (ImGui::Button("Add Box", ImVec2(-1, 0))) {
            Mesh newMesh("Box_" + std::to_string(meshes.size() + 1));
            GeometryFactory::createBox(newMesh);
            meshes.push_back(newMesh);
            originalMeshes.push_back(newMesh);
            selectedMesh = meshes.size() - 1;
            LogManager::getInstance()->logOperation("Object Creation", "Box created: " + newMesh.name);
        }
        if (ImGui::Button("Add Cylinder", ImVec2(-1, 0))) {
            Mesh newMesh("Cylinder_" + std::to_string(meshes.size() + 1));
            GeometryFactory::createCylinder(newMesh);
            meshes.push_back(newMesh);
            originalMeshes.push_back(newMesh);
            selectedMesh = meshes.size() - 1;
            LogManager::getInstance()->logOperation("Object Creation", "Cylinder created: " + newMesh.name);
        }
        
        ImGui::Separator();
        
        // 对象列表
        for (size_t i = 0; i < meshes.size(); i++) {
            bool isSelected = (static_cast<int>(i) == selectedMesh);
            if (ImGui::Selectable(meshes[i].name.c_str(), isSelected)) {
                for (auto& mesh : meshes) {
                    mesh.selected = false;
                }
                selectedMesh = static_cast<int>(i);
                meshes[i].selected = true;
            }
        }
        
        ImGui::EndTabItem();
    }
    
    if (ImGui::BeginTabItem("Properties")) {
        if (selectedMesh >= 0 && selectedMesh < static_cast<int>(meshes.size())) {
            ImGui::Text("Object Properties");
            ImGui::Text("Name: %s", meshes[selectedMesh].name.c_str());
            
            ImGui::Separator();
            ImGui::Text("Transform");
            
            glm::vec3 translation = glm::vec3(meshes[selectedMesh].transform[3]);
            if (ImGui::DragFloat3("Position", glm::value_ptr(translation), 0.1f)) {
                meshes[selectedMesh].transform[3] = glm::vec4(translation, 1.0f);
                LogManager::getInstance()->logOperation("Object Transformation", "Position changed for " + meshes[selectedMesh].name + ": (" + std::to_string(translation.x) + ", " + std::to_string(translation.y) + ", " + std::to_string(translation.z) + ")");
            }
            
            glm::vec3 rotation = TransformController().getCurrentRotation(meshes[selectedMesh]);
            if (ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 1.0f, -180.0f, 180.0f)) {
                TransformController::setRotation(meshes[selectedMesh], rotation);
                LogManager::getInstance()->logOperation("Object Transformation", "Rotation changed for " + meshes[selectedMesh].name + ": (" + std::to_string(rotation.x) + ", " + std::to_string(rotation.y) + ", " + std::to_string(rotation.z) + ")");
            }
            
            glm::vec3 scale = TransformController().getCurrentScale(meshes[selectedMesh]);
            if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f, 0.1f, 10.0f)) {
                TransformController::setScale(meshes[selectedMesh], scale);
                LogManager::getInstance()->logOperation("Object Transformation", "Scale changed for " + meshes[selectedMesh].name + ": (" + std::to_string(scale.x) + ", " + std::to_string(scale.y) + ", " + std::to_string(scale.z) + ")");
            }
            
            ImGui::Separator();
            if (ImGui::ColorEdit3("Color", glm::value_ptr(meshes[selectedMesh].baseColor))) {
                for (auto& vertex : meshes[selectedMesh].vertices) {
                    vertex.color = meshes[selectedMesh].baseColor;
                }
                if (meshes[selectedMesh].VBO != 0) {
                    glBindBuffer(GL_ARRAY_BUFFER, meshes[selectedMesh].VBO);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, meshes[selectedMesh].vertices.size() * sizeof(Vertex), meshes[selectedMesh].vertices.data());
                }
            }
            
            ImGui::Separator();
            if (ImGui::Button("Delete Object", ImVec2(-1, 0))) {
                if (selectedMesh >= 0 && selectedMesh < static_cast<int>(meshes.size())) {
                    std::string deletedObjectName = meshes[selectedMesh].name;
                    meshes.erase(meshes.begin() + selectedMesh);
                    originalMeshes.erase(originalMeshes.begin() + selectedMesh);
                    LogManager::getInstance()->logOperation("Object Deletion", "Object deleted: " + deletedObjectName);
                    selectedMesh = -1;
                }
            }
        } else {
            ImGui::Text("No object selected");
        }
        
        ImGui::EndTabItem();
    }
    
    if (ImGui::BeginTabItem("Boolean")) {
        if (selectedMesh >= 0 && selectedMesh < static_cast<int>(meshes.size())) {
            ImGui::Text("Boolean Operations");
            ImGui::Text("Selected: %s", meshes[selectedMesh].name.c_str());
            
            if (secondMeshForBoolean >= 0) {
                ImGui::Text("Second: %s", meshes[secondMeshForBoolean].name.c_str());
            }
            
            if (secondMeshForBoolean >= 0 && secondMeshForBoolean != selectedMesh) {
                if (ImGui::Button("Union (U)", ImVec2(-1, 0))) {
                    performBooleanOperation(BooleanOperation::UNION);
                }
                if (ImGui::Button("Difference (D)", ImVec2(-1, 0))) {
                    performBooleanOperation(BooleanOperation::DIFFERENCE);
                }
                if (ImGui::Button("Intersection (I)", ImVec2(-1, 0))) {
                    performBooleanOperation(BooleanOperation::INTERSECTION);
                }
            } else {
                ImGui::Text("Select second object");
            }
            
            if (ImGui::Button("Set as Second Object", ImVec2(-1, 0))) {
                secondMeshForBoolean = selectedMesh;
            }
        } else {
            ImGui::Text("Select an object first");
        }
        
        ImGui::EndTabItem();
    }
    
    ImGui::EndTabBar();
    
    ImGui::End();
    
    // Command Window - 只在选中几何体且有键盘输入时显示
    if (selectedMesh >= 0 && selectedMesh < static_cast<int>(meshes.size())) {
        ImGui::SetNextWindowPos(ImVec2(10, ImGui::GetMainViewport()->Size.y - 200), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x - 20, 180), ImGuiCond_FirstUseEver);
        ImGui::Begin("Command Window", nullptr, ImGuiWindowFlags_NoCollapse);
        
        // 显示选中几何体的属性
        ImGui::Text("Selected Object: %s", meshes[selectedMesh].name.c_str());
        ImGui::Separator();
        
        // 获取对象的位置、缩放和旋转信息
        glm::vec3 position = glm::vec3(meshes[selectedMesh].transform[3]);
        glm::vec3 scale = glm::vec3(
            glm::length(glm::vec3(meshes[selectedMesh].transform[0])),
            glm::length(glm::vec3(meshes[selectedMesh].transform[1])),
            glm::length(glm::vec3(meshes[selectedMesh].transform[2]))
        );
        
        ImGui::Text("Position: (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
        ImGui::Text("Scale: (%.2f, %.2f, %.2f)", scale.x, scale.y, scale.z);
        ImGui::Text("Material Color: (%.2f, %.2f, %.2f)", 
                   meshes[selectedMesh].baseColor.r, 
                   meshes[selectedMesh].baseColor.g, 
                   meshes[selectedMesh].baseColor.b);
        
        ImGui::Separator();
        
        // 输入命令区域
        ImGui::Text("Enter command (e.g., translate, scale, rotate): ");
        bool submitted = ImGui::InputText("##CommandInput", commandInput, sizeof(commandInput), ImGuiInputTextFlags_EnterReturnsTrue);
        
        if (ImGui::Button("Execute") || submitted) {
            if (strlen(commandInput) > 0) {
                if (commandParser.parseAndExecute(std::string(commandInput), meshes)) {
                    LogManager::getInstance()->logOperation("Command Execution", std::string("Executed: ") + commandInput);
                    memset(commandInput, 0, sizeof(commandInput)); // 清空输入框
                } else {
                    LogManager::getInstance()->logWarning(std::string("Invalid command: ") + commandInput);
                }
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear")) {
            memset(commandInput, 0, sizeof(commandInput));
        }
        
        // 显示命令历史
        const auto& history = commandParser.getCommandHistory();
        if (!history.empty()) {
            ImGui::Separator();
            ImGui::Text("Recent commands:");
            ImGui::BeginChild("CommandHistory", ImVec2(0, 40), true);
            for (int i = history.size() - 1; i >= 0 && i >= (int)history.size() - 5; i--) {  // 显示最近5条
                ImGui::Text("%s", history[i].c_str());
            }
            ImGui::EndChild();
        }
        
        ImGui::End();
    }
    
    // 状态栏 - 确保在非DockSpace模式下也能显示
    ImGuiViewport* statusViewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(statusViewport->Pos.x, statusViewport->Pos.y + statusViewport->Size.y - 20));
    ImGui::SetNextWindowSize(ImVec2(statusViewport->Size.x, 20));
    ImGui::Begin("StatusBar", nullptr, 
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoInputs);
    
    ImGui::Text("3D Modeling Tool | %d objects | %.1f FPS", 
                (int)meshes.size(), ImGui::GetIO().Framerate);

    ImGui::End();

    // 显示文件对话框
    if (fileDialog.Display("ChooseFileDlgKey")) {
        // 如果用户点击了确定
        if (fileDialog.IsOk()) {
            std::string filePathName = fileDialog.GetFilePathName();
            saveScene(filePathName);
            LogManager::getInstance()->logOperation("File Operation", "Scene saved to " + filePathName);
        }
        fileDialog.Close();
    }
    
    // 显示打开文件对话框
    if (fileDialog.Display("ChooseFileToOpenDlgKey")) {
        // 如果用户点击了确定
        if (fileDialog.IsOk()) {
            std::string filePathName = fileDialog.GetFilePathName();
            loadScene(filePathName);
            LogManager::getInstance()->logOperation("File Operation", "Scene loaded from " + filePathName);
        }
        fileDialog.Close();
    }

    // 快捷键设置窗口
    static bool showKeyBindingsWindow = false;
    if (showKeyBindingsWindow) {
        ImGui::Begin("Key Bindings", &showKeyBindingsWindow);
        
        // 显示和修改快捷键设置
        // 这里只显示部分，实际应用中应该全部显示并允许修改
        
        ImGui::Text("Camera Controls");
        ImGui::Text("Rotate: %s", keyBindings.rotateCamera.c_str());
        ImGui::Text("Pan: %s", keyBindings.panCamera.c_str());
        
        ImGui::Separator();
        ImGui::Text("Object Creation");
        ImGui::Text("Cube: %s", keyBindings.createCube.c_str());
        ImGui::Text("Sphere: %s", keyBindings.createSphere.c_str());
        ImGui::Text("Cylinder: %s", keyBindings.createCylinder.c_str());
        ImGui::Text("Plane: %s", keyBindings.createPlane.c_str());
        ImGui::Text("Line: %s", keyBindings.createLine.c_str());
        ImGui::Text("Point: %s", keyBindings.createPoint.c_str());
        
        ImGui::Separator();
        ImGui::Text("Boolean Operations");
        ImGui::Text("Union: %s", keyBindings.booleanUnion.c_str());
        ImGui::Text("Difference: %s", keyBindings.booleanDifference.c_str());
        ImGui::Text("Intersection: %s", keyBindings.booleanIntersection.c_str());
        
        ImGui::Separator();
        ImGui::Text("File Operations");
        ImGui::Text("Save: %s", keyBindings.saveScene.c_str());
        ImGui::Text("Load: %s", keyBindings.loadScene.c_str());
        ImGui::Text("Exit: %s", keyBindings.exitApp.c_str());
        
        if (ImGui::Button("Close")) {
            showKeyBindingsWindow = false;
        }
        
        ImGui::End();
    }
}