#pragma once

#include <glad/glad.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ImGuiFileDialog.h"

#include "../MWindows.h"
#include "../../io/config_manager.h"
#include "../../io/scene_manager.h"
#include "../../core/log_manager.h"
#include "ViewportWindow.h"

namespace mcnp::ui {

// 说明：
// - 顶部栏使用 ImGui::BeginMainMenuBar()，不再占用一个固定高度窗口，
//   以适配 Docking 并满足“可拖拽/可缩放”窗口的需求。
// - 仍保留 MWindows 基类，便于后续统一管理，但默认不再通过 Draw() 渲染。
class TopBarWindow final : public MWindows {
public:
    TopBarWindow() : MWindows("##TopBar")
    {
        SetClosable(false);
        SetForceApplyLayout(false);
        SetFlags(ImGuiWindowFlags_NoSavedSettings);
    }

    void SetWindows(MWindows* side, MWindows* bottom, ViewportWindow* viewport) noexcept
    {
        side_ = side;
        bottom_ = bottom;
        viewport_ = viewport;
    }

    // 顶部主菜单栏入口（由 UILayoutManager 调用）
    void DrawMainMenuBar()
    {
        if (!ImGui::BeginMainMenuBar()) {
            return;
        }

        DrawMenus();
        DrawRightStatus();
        ImGui::EndMainMenuBar();

        DrawPopups();
        DrawAbout();
    }

private:
    void OnDraw() override {} // 不使用

    void DrawMenus()
    {
        if (ImGui::BeginMenu("文件")) {
            if (ImGui::MenuItem("新建")) {
                meshes.clear();
                originalMeshes.clear();
                selectedMesh = -1;
                secondMeshForBoolean = -1;
                LogManager::getInstance()->logOperation("File", "New scene");
            }
            if (ImGui::MenuItem("打开...")) {
                OpenFileDialog("ChooseFileToOpenDlgKey", "Choose Scene File");
            }
            if (ImGui::MenuItem("保存")) {
                saveScene(savePath_);
                LogManager::getInstance()->logOperation("File", std::string("Save scene: ") + savePath_);
            }
            if (ImGui::MenuItem("另存为...")) {
                OpenFileDialog("ChooseFileToSaveDlgKey", "Save Scene As");
            }
            ImGui::Separator();
            if (ImGui::MenuItem("退出")) {
                LogManager::getInstance()->logInfo("Exit requested");
                glfwSetWindowShouldClose(window, true);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("编辑")) {
            ImGui::MenuItem("撤销", "Ctrl+Z", false, false);
            ImGui::MenuItem("重做", "Ctrl+Y", false, false);
            if (ImGui::MenuItem("删除", "Del", false, selectedMesh >= 0 && selectedMesh < (int)meshes.size())) {
                const std::string name = meshes[selectedMesh].name;
                meshes.erase(meshes.begin() + selectedMesh);
                originalMeshes.erase(originalMeshes.begin() + selectedMesh);
                selectedMesh = -1;
                LogManager::getInstance()->logOperation("Edit", "Delete object: " + name);
            }
            if (ImGui::MenuItem("复制", "Ctrl+D", false, selectedMesh >= 0 && selectedMesh < (int)meshes.size())) {
                Mesh copy = meshes[selectedMesh];
                copy.name += "_Copy";
                copy.selected = false;
                // 复制出来的 Mesh 需要重新生成 GPU 资源
                copy.VAO = copy.VBO = copy.EBO = 0;
                meshes.push_back(copy);
                originalMeshes.push_back(copy);
                LogManager::getInstance()->logOperation("Edit", "Duplicate: " + copy.name);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("选择")) {
            if (ImGui::MenuItem("取消选择")) {
                for (auto& mesh : meshes) mesh.selected = false;
                selectedMesh = -1;
                LogManager::getInstance()->logOperation("Select", "Select none");
            }
            if (ImGui::MenuItem("全选")) {
                for (auto& mesh : meshes) mesh.selected = true;
                selectedMesh = meshes.empty() ? -1 : 0;
                LogManager::getInstance()->logOperation("Select", "Select all");
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("视图")) {
            if (ImGui::MenuItem("显示网格", nullptr, &sceneState.showGrid)) {
                LogManager::getInstance()->logOperation("View", "Toggle grid");
            }
            if (ImGui::MenuItem("重置相机")) {
                sceneState.cameraAngles = glm::vec2(0.0f, 0.0f);
                sceneState.cameraDistance = 5.0f;
                sceneState.cameraTarget = glm::vec3(0.0f);
                LogManager::getInstance()->logOperation("View", "Reset camera");
            }
            ImGui::Separator();
            if (ImGui::MenuItem("重置布局")) {
                requestResetLayout_ = true;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("工具")) {
            if (ImGui::MenuItem("立方体")) { CreateFromBuiltin(CUBE_MODEL_FILE.c_str(), "Cube"); }
            if (ImGui::MenuItem("球体")) { CreateFromBuiltin(SPHERE_MODEL_FILE.c_str(), "Sphere"); }
            if (ImGui::MenuItem("圆柱体")) { CreateFromBuiltin(CYLINDER_MODEL_FILE.c_str(), "Cylinder"); }
            if (ImGui::MenuItem("平面")) { CreateFromBuiltin(PLANE_MODEL_FILE.c_str(), "Plane"); }
            ImGui::Separator();
            if (ImGui::MenuItem("布尔并集")) { performBooleanOperation(BooleanOperation::UNION); LogManager::getInstance()->logOperation("Tools", "Boolean union"); }
            if (ImGui::MenuItem("布尔差集")) { performBooleanOperation(BooleanOperation::DIFFERENCE); LogManager::getInstance()->logOperation("Tools", "Boolean difference"); }
            if (ImGui::MenuItem("布尔交集")) { performBooleanOperation(BooleanOperation::INTERSECTION); LogManager::getInstance()->logOperation("Tools", "Boolean intersection"); }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("设置")) {
            if (ImGui::SliderInt("FPS 限制", &sceneState.fpsLimit, 30, 240, "%d FPS")) {
                LogManager::getInstance()->logOperation("Settings", "FPS limit change");
            }
            if (ImGui::SliderFloat("字体大小", &sceneState.uiFontSize, 12.0f, 24.0f, "%.1f")) {
                updateUIFontSize(sceneState.uiFontSize);
                LogManager::getInstance()->logOperation("Settings", "Font size change");
            }
            ImGui::Separator();
            if (ImGui::Checkbox("控制台输出日志", &consoleLogEnabled_)) {
                LogManager::getInstance()->setConsoleEnabled(consoleLogEnabled_);
                LogManager::getInstance()->logOperation("Settings", std::string("Console log ") + (consoleLogEnabled_ ? "ON" : "OFF"));
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("窗口")) {
            if (side_) ImGui::MenuItem("侧边栏", nullptr, side_->VisiblePtr());
            if (bottom_) ImGui::MenuItem("底部栏", nullptr, bottom_->VisiblePtr());
            if (viewport_) ImGui::MenuItem("Viewport", nullptr, viewport_->VisiblePtr());
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("帮助")) {
            if (ImGui::MenuItem("关于")) {
                showAbout_ = true;
            }
            ImGui::EndMenu();
        }
    }

    void DrawRightStatus()
    {
        const float fps = ImGui::GetIO().Framerate;
        ImVec2 vpSize(0.0f, 0.0f);
        if (viewport_) {
            vpSize = viewport_->ViewportSize();
        }

        ImVec2 mouse = ImGui::GetIO().MousePos;
        ImVec2 mouseLocal(-1.0f, -1.0f);
        bool mouseInViewport = false;
        if (viewport_) {
            ImVec2 imgPos, imgSize;
            if (viewport_->GetImageRect(imgPos, imgSize)) {
                if (mouse.x >= imgPos.x && mouse.y >= imgPos.y &&
                    mouse.x <= imgPos.x + imgSize.x && mouse.y <= imgPos.y + imgSize.y) {
                    mouseInViewport = true;
                    mouseLocal = ImVec2(mouse.x - imgPos.x, mouse.y - imgPos.y);
                }
            }
        }

        std::string selectedInfo = "未选中";
        if (selectedMesh >= 0 && selectedMesh < static_cast<int>(meshes.size())) {
            const Mesh& mesh = meshes[selectedMesh];
            const int triCount = mesh.indices.empty() ? 0 : static_cast<int>(mesh.indices.size() / 3);
            const glm::vec3 pos = glm::vec3(mesh.transform[3]);
            selectedInfo = mesh.name +
                "  V:" + std::to_string(mesh.vertices.size()) +
                "  T:" + std::to_string(triCount) +
                "  P:(" + std::to_string(pos.x) + "," + std::to_string(pos.y) + "," + std::to_string(pos.z) + ")";
        }

        // 右对齐：先计算需要的宽度
        char buf[512] = {};
        if (mouseInViewport) {
            std::snprintf(buf, sizeof(buf), "FPS %.1f | 视图 %dx%d | 鼠标 %.0f,%.0f | %s",
                          fps, (int)vpSize.x, (int)vpSize.y, mouseLocal.x, mouseLocal.y, selectedInfo.c_str());
        } else {
            std::snprintf(buf, sizeof(buf), "FPS %.1f | 视图 %dx%d | 鼠标 -,- | %s",
                          fps, (int)vpSize.x, (int)vpSize.y, selectedInfo.c_str());
        }

        const float textWidth = ImGui::CalcTextSize(buf).x;
        ImGui::SameLine(ImGui::GetWindowWidth() - textWidth - 12.0f);
        ImGui::TextUnformatted(buf);
    }

    void DrawPopups()
    {
        if (fileDialog_.Display("ChooseFileToOpenDlgKey")) {
            if (fileDialog_.IsOk()) {
                const std::string path = fileDialog_.GetFilePathName();
                std::snprintf(openPath_, sizeof(openPath_), "%s", path.c_str());
                loadScene(openPath_);
                LogManager::getInstance()->logOperation("File", std::string("Open scene: ") + openPath_);
            }
            fileDialog_.Close();
        }

        if (fileDialog_.Display("ChooseFileToSaveDlgKey")) {
            if (fileDialog_.IsOk()) {
                const std::string path = fileDialog_.GetFilePathName();
                std::snprintf(savePath_, sizeof(savePath_), "%s", path.c_str());
                saveScene(savePath_);
                LogManager::getInstance()->logOperation("File", std::string("Save scene: ") + savePath_);
            }
            fileDialog_.Close();
        }
    }

    void DrawAbout()
    {
        if (!showAbout_) return;
        if (ImGui::Begin("关于 MCNPApp", &showAbout_, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextUnformatted("MCNP 可视化编辑器（开发中）");
            ImGui::Separator();
            ImGui::Text("Objects: %d", (int)meshes.size());
            ImGui::Text("OpenGL: %s", glGetString(GL_VERSION));
            ImGui::TextUnformatted("快捷键：Ctrl+鼠标滚轮缩放 / 右键旋转（待完善）");
        }
        ImGui::End();
    }

    void CreateFromBuiltin(const char* modelPath, const char* prefix)
    {
        Mesh newMesh;
        if (loadModelFromFile(newMesh, modelPath)) {
            newMesh.name = std::string(prefix) + "_" + std::to_string(meshes.size() + 1);
            meshes.push_back(newMesh);
            originalMeshes.push_back(newMesh);
            selectedMesh = static_cast<int>(meshes.size()) - 1;
            for (auto& m : meshes) m.selected = false;
            meshes[selectedMesh].selected = true;
            LogManager::getInstance()->logOperation("Tools", std::string("Create ") + prefix + ": " + newMesh.name);
        }
    }

public:
    // 给外部布局管理器读取/消耗请求
    bool ConsumeResetLayoutRequest() noexcept
    {
        const bool v = requestResetLayout_;
        requestResetLayout_ = false;
        return v;
    }
    bool ConsoleLogEnabled() const noexcept { return consoleLogEnabled_; }

private:
    void OpenFileDialog(const char* key, const char* title)
    {
        IGFD::FileDialogConfig config;
        config.path = "user/scenes/";

        ImVec2 dialogSize = ImGui::GetMainViewport()->Size;
        dialogSize.x *= 0.7f;
        dialogSize.y *= 0.6f;
        ImGui::SetNextWindowSize(dialogSize);

        fileDialog_.OpenDialog(key, title, ".json,.JSON", config);
    }

    MWindows* side_{nullptr};
    MWindows* bottom_{nullptr};
    ViewportWindow* viewport_{nullptr};

    inline static char openPath_[256] = "user/scenes/scene.json";
    inline static char savePath_[256] = "user/scenes/scene.json";
    inline static IGFD::FileDialog fileDialog_;

    bool showAbout_{false};
    bool requestResetLayout_{false};
    bool consoleLogEnabled_{true};
};

} // namespace mcnp::ui
