#pragma once
#include "../MWindows.h"
#include "../../io/config_manager.h"
#include "../../io/scene_manager.h"
#include "../../core/log_manager.h"
#include "../transform_controller.h"

namespace mcnp::ui {

class SideBarWindow final : public MWindows {
public:
    SideBarWindow()
        : MWindows("Side Panel")
    {
        // Docking 下允许用户移动/缩放窗口，不再禁止 Move。
        SetFlags(ImGuiWindowFlags_NoCollapse);
        SetClosable(false);
    }

    void SetWidth(float w) noexcept { width_ = w; }
    float Width() const noexcept { return width_; }

private:
    void OnDraw() override
    {
        width_ = ImGui::GetWindowSize().x;

        if (ImGui::BeginTabBar("SideBarTabs")) {
            if (ImGui::BeginTabItem("View")) {
                if (ImGui::Checkbox("Show Grid", &sceneState.showGrid)) {
                    LogManager::getInstance()->logOperation("View", "Toggle grid");
                }
                if (ImGui::ColorEdit3("Grid Color", glm::value_ptr(sceneState.gridColor))) {
                    LogManager::getInstance()->logOperation("View", "Grid color change");
                }

                ImGui::Separator();
                ImGui::Text("Camera");
                ImGui::Text("Distance: %.2f", sceneState.cameraDistance);
                ImGui::Text("Angle X: %.2f", sceneState.cameraAngles.x);
                ImGui::Text("Angle Y: %.2f", sceneState.cameraAngles.y);

                ImGui::Separator();
                if (ImGui::SliderInt("FPS Limit", &sceneState.fpsLimit, 30, 240, "%d FPS")) {
                    LogManager::getInstance()->logOperation("View", "FPS limit change");
                }
                if (ImGui::SliderFloat("Font Size", &sceneState.uiFontSize, 20.0f, 52.0f, "%.1f")) {
                    updateUIFontSize(sceneState.uiFontSize);
                    LogManager::getInstance()->logOperation("View", "Font size change");
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Objects")) {
                ImGui::Text("Objects (%d)", (int)meshes.size());

                if (ImGui::Button("Add Cube", ImVec2(-1, 0))) {
                    Mesh newMesh;
                    if (loadModelFromFile(newMesh, CUBE_MODEL_FILE)) {
                        newMesh.name = "Cube_" + std::to_string(meshes.size() + 1);
                        meshes.push_back(newMesh);
                        originalMeshes.push_back(newMesh);
                        selectedMesh = (int)meshes.size() - 1;
                        LogManager::getInstance()->logOperation("Objects", "Add cube: " + newMesh.name);
                    }
                }
                if (ImGui::Button("Add Sphere", ImVec2(-1, 0))) {
                    Mesh newMesh;
                    if (loadModelFromFile(newMesh, SPHERE_MODEL_FILE)) {
                        newMesh.name = "Sphere_" + std::to_string(meshes.size() + 1);
                        meshes.push_back(newMesh);
                        originalMeshes.push_back(newMesh);
                        selectedMesh = (int)meshes.size() - 1;
                        LogManager::getInstance()->logOperation("Objects", "Add sphere: " + newMesh.name);
                    }
                }
                if (ImGui::Button("Add Cylinder", ImVec2(-1, 0))) {
                    Mesh newMesh;
                    if (loadModelFromFile(newMesh, CYLINDER_MODEL_FILE)) {
                        newMesh.name = "Cylinder_" + std::to_string(meshes.size() + 1);
                        meshes.push_back(newMesh);
                        originalMeshes.push_back(newMesh);
                        selectedMesh = (int)meshes.size() - 1;
                        LogManager::getInstance()->logOperation("Objects", "Add cylinder: " + newMesh.name);
                    }
                }
                if (ImGui::Button("Add Plane", ImVec2(-1, 0))) {
                    Mesh newMesh;
                    if (loadModelFromFile(newMesh, PLANE_MODEL_FILE)) {
                        newMesh.name = "Plane_" + std::to_string(meshes.size() + 1);
                        meshes.push_back(newMesh);
                        originalMeshes.push_back(newMesh);
                        selectedMesh = (int)meshes.size() - 1;
                        LogManager::getInstance()->logOperation("Objects", "Add plane: " + newMesh.name);
                    }
                }
                if (ImGui::Button("Add Line", ImVec2(-1, 0))) {
                    Mesh newMesh("Line_" + std::to_string(meshes.size() + 1));
                    createLineMesh(newMesh);
                    meshes.push_back(newMesh);
                    originalMeshes.push_back(newMesh);
                    selectedMesh = (int)meshes.size() - 1;
                    LogManager::getInstance()->logOperation("Objects", "Add line: " + newMesh.name);
                }
                if (ImGui::Button("Add Point", ImVec2(-1, 0))) {
                    Mesh newMesh("Point_" + std::to_string(meshes.size() + 1));
                    createPointMesh(newMesh);
                    meshes.push_back(newMesh);
                    originalMeshes.push_back(newMesh);
                    selectedMesh = (int)meshes.size() - 1;
                    LogManager::getInstance()->logOperation("Objects", "Add point: " + newMesh.name);
                }

                ImGui::Separator();
                for (int i = 0; i < (int)meshes.size(); i++) {
                    bool isSelected = (selectedMesh == i);
                    if (ImGui::Selectable(meshes[i].name.c_str(), isSelected)) {
                        for (auto& mesh : meshes) mesh.selected = false;
                        selectedMesh = i;
                        meshes[i].selected = true;
                        LogManager::getInstance()->logOperation("Objects", "Select: " + meshes[i].name);
                    }
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Properties")) {
                if (selectedMesh >= 0 && selectedMesh < (int)meshes.size()) {
                    Mesh& mesh = meshes[selectedMesh];
                    ImGui::Text("Name: %s", mesh.name.c_str());
                    ImGui::Text("Vertices: %d", (int)mesh.vertices.size());
                    ImGui::Text("Indices: %d", (int)mesh.indices.size());

                    ImGui::Separator();
                    glm::vec3 translation = glm::vec3(mesh.transform[3]);
                    if (ImGui::DragFloat3("Position", glm::value_ptr(translation), 0.1f)) {
                        mesh.transform[3] = glm::vec4(translation, 1.0f);
                        LogManager::getInstance()->logOperation("Properties", "Position change: " + mesh.name);
                    }

                    glm::vec3 rotation = TransformController().getCurrentRotation(mesh);
                    if (ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 1.0f, -180.0f, 180.0f)) {
                        TransformController::setRotation(mesh, rotation);
                        LogManager::getInstance()->logOperation("Properties", "Rotation change: " + mesh.name);
                    }

                    glm::vec3 scale = TransformController().getCurrentScale(mesh);
                    if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f, 0.1f, 10.0f)) {
                        TransformController::setScale(mesh, scale);
                        LogManager::getInstance()->logOperation("Properties", "Scale change: " + mesh.name);
                    }

                    if (ImGui::ColorEdit3("Color", glm::value_ptr(mesh.baseColor))) {
                        for (auto& vertex : mesh.vertices) {
                            vertex.color = mesh.baseColor;
                        }
                        if (mesh.VBO != 0) {
                            glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
                            glBufferSubData(GL_ARRAY_BUFFER, 0, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data());
                        }
                        LogManager::getInstance()->logOperation("Properties", "Color change: " + mesh.name);
                    }

                    if (ImGui::Button("Delete Object", ImVec2(-1, 0))) {
                        std::string deletedName = mesh.name;
                        meshes.erase(meshes.begin() + selectedMesh);
                        originalMeshes.erase(originalMeshes.begin() + selectedMesh);
                        selectedMesh = -1;
                        LogManager::getInstance()->logOperation("Properties", "Delete: " + deletedName);
                    }
                } else {
                    ImGui::Text("No object selected");
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Boolean")) {
                if (selectedMesh >= 0 && selectedMesh < (int)meshes.size()) {
                    ImGui::Text("Selected: %s", meshes[selectedMesh].name.c_str());
                    if (secondMeshForBoolean >= 0 && secondMeshForBoolean < (int)meshes.size()) {
                        ImGui::Text("Second: %s", meshes[secondMeshForBoolean].name.c_str());
                    }
                    if (secondMeshForBoolean >= 0 && secondMeshForBoolean != selectedMesh) {
                        if (ImGui::Button("Union", ImVec2(-1, 0))) {
                            performBooleanOperation(BooleanOperation::UNION);
                            LogManager::getInstance()->logOperation("Boolean", "Union");
                        }
                        if (ImGui::Button("Difference", ImVec2(-1, 0))) {
                            performBooleanOperation(BooleanOperation::DIFFERENCE);
                            LogManager::getInstance()->logOperation("Boolean", "Difference");
                        }
                        if (ImGui::Button("Intersection", ImVec2(-1, 0))) {
                            performBooleanOperation(BooleanOperation::INTERSECTION);
                            LogManager::getInstance()->logOperation("Boolean", "Intersection");
                        }
                    } else {
                        ImGui::Text("Select second object");
                    }
                    if (ImGui::Button("Set as Second", ImVec2(-1, 0))) {
                        secondMeshForBoolean = selectedMesh;
                        LogManager::getInstance()->logOperation("Boolean", "Set second object");
                    }
                } else {
                    ImGui::Text("Select an object first");
                }
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }

    float width_{320.0f};
};

} // namespace mcnp::ui
