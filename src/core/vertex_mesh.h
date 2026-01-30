#ifndef VERTEX_MESH_H
#define VERTEX_MESH_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <functional>
#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// 顶点结构
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    
    Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec3 col) 
        : position(pos), normal(norm), color(col) {}
};

// 网格结构
struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    glm::mat4 transform;
    glm::vec3 baseColor;
    std::string name;
    bool selected;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    
    Mesh(std::string n = "Object") : 
        transform(glm::mat4(1.0f)), 
        baseColor(0.8f, 0.5f, 0.2f),
        name(n),
        selected(false),
        VAO(0),
        VBO(0),
        EBO(0) {}
};

// 释放网格资源
inline void releaseMeshResources(Mesh& mesh) {
    if (mesh.VAO != 0) {
        glDeleteVertexArrays(1, &mesh.VAO);
        mesh.VAO = 0;
    }
    if (mesh.VBO != 0) {
        glDeleteBuffers(1, &mesh.VBO);
        mesh.VBO = 0;
    }
    if (mesh.EBO != 0) {
        glDeleteBuffers(1, &mesh.EBO);
        mesh.EBO = 0;
    }
}

// 布尔运算类型
enum class BooleanOperation {
    UNION,
    DIFFERENCE,
    INTERSECTION
};

// 创建基本几何体网格的函数
void createCubeMesh(Mesh& mesh);
void createSphereMesh(Mesh& mesh, int segments = 32);
void createCylinderMesh(Mesh& mesh, float radius = 0.5f, float height = 1.0f, int segments = 32);
void createPlaneMesh(Mesh& mesh, float width = 1.0f, float height = 1.0f);
void createLineMesh(Mesh& mesh, glm::vec3 start = glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec3 end = glm::vec3(0.5f, 0.0f, 0.0f));
void createPointMesh(Mesh& mesh, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
void createCircleMesh(Mesh& mesh, float radius = 0.5f, int segments = 32);

// 模型文件加载和保存
bool loadModelFromFile(Mesh& mesh, const std::string& filename);
bool saveModelToFile(const Mesh& mesh, const std::string& filename);

#endif // VERTEX_MESH_H