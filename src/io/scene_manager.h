#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <vector>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include "vertex_mesh.h"
#include "config_manager.h"  // 包含SceneState和KeyBindings的定义

using json = nlohmann::json;

// 新增：建模文件路径
extern const std::string CUBE_MODEL_FILE;
extern const std::string SPHERE_MODEL_FILE;
extern const std::string CYLINDER_MODEL_FILE;
extern const std::string PLANE_MODEL_FILE;

// 全局变量声明
extern std::vector<Mesh> meshes;
extern std::vector<Mesh> originalMeshes;
extern int selectedMesh;
extern int secondMeshForBoolean;
extern SceneState sceneState;

// 场景管理相关函数声明
void saveScene(const std::string& filename);
void loadScene(const std::string& filename);
void performBooleanOperation(BooleanOperation operation);

#endif