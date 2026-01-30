#include "vertex_mesh.h"
#include <cmath>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

// 创建立方体网格
void createCubeMesh(Mesh& mesh)
{
    mesh.vertices = {
        // 前面
        Vertex({-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, mesh.baseColor),
        Vertex({0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, mesh.baseColor),
        Vertex({0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, mesh.baseColor),
        Vertex({-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, mesh.baseColor),
        
        // 后面
        Vertex({-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, mesh.baseColor),
        Vertex({0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, mesh.baseColor),
        Vertex({0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, mesh.baseColor),
        Vertex({-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, mesh.baseColor),
        
        // 左面
        Vertex({-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, mesh.baseColor),
        Vertex({-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, mesh.baseColor),
        Vertex({-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, mesh.baseColor),
        Vertex({-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, mesh.baseColor),
        
        // 右面
        Vertex({0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, mesh.baseColor),
        Vertex({0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, mesh.baseColor),
        Vertex({0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, mesh.baseColor),
        Vertex({0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, mesh.baseColor),
        
        // 上面
        Vertex({-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, mesh.baseColor),
        Vertex({0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, mesh.baseColor),
        Vertex({0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, mesh.baseColor),
        Vertex({-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, mesh.baseColor),
        
        // 下面
        Vertex({-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, mesh.baseColor),
        Vertex({0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, mesh.baseColor),
        Vertex({0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, mesh.baseColor),
        Vertex({-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, mesh.baseColor)
    };
    
    mesh.indices = {
        // 前面
        0, 1, 2, 2, 3, 0,
        // 后面
        4, 5, 6, 6, 7, 4,
        // 左面
        8, 9, 10, 10, 11, 8,
        // 右面
        12, 13, 14, 14, 15, 12,
        // 上面
        16, 17, 18, 18, 19, 16,
        // 下面
        20, 21, 22, 22, 23, 20
    };
}

// 创建球体网格
void createSphereMesh(Mesh& mesh, int segments)
{
    mesh.vertices.clear();
    mesh.indices.clear();
    
    // 生成球体顶点
    for (int i = 0; i <= segments; i++) {
        float phi = glm::pi<float>() * i / segments;  // 从0到π
        
        for (int j = 0; j <= segments; j++) {
            float theta = 2.0f * glm::pi<float>() * j / segments;  // 从0到2π
            
            float x = sin(phi) * cos(theta);
            float y = cos(phi);
            float z = sin(phi) * sin(theta);
            
            glm::vec3 pos(x, y, z);
            glm::vec3 norm = glm::normalize(pos);  // 法线方向与位置相同
            
            mesh.vertices.push_back(Vertex(pos, norm, mesh.baseColor));
        }
    }
    
    // 生成索引
    for (int i = 0; i < segments; i++) {
        for (int j = 0; j < segments; j++) {
            int first = i * (segments + 1) + j;
            int second = first + segments + 1;
            
            mesh.indices.push_back(first);
            mesh.indices.push_back(second);
            mesh.indices.push_back(first + 1);
            
            mesh.indices.push_back(second);
            mesh.indices.push_back(second + 1);
            mesh.indices.push_back(first + 1);
        }
    }
}

// 创建圆柱体网格
void createCylinderMesh(Mesh& mesh, float radius, float height, int segments)
{
    mesh.vertices.clear();
    mesh.indices.clear();
    
    // 顶部和底部中心点
    mesh.vertices.push_back(Vertex({0, height/2, 0}, {0, 1, 0}, mesh.baseColor)); // 顶部中心
    mesh.vertices.push_back(Vertex({0, -height/2, 0}, {0, -1, 0}, mesh.baseColor)); // 底部中心
    
    // 创建侧面
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * glm::pi<float>() * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        
        // 顶部顶点
        mesh.vertices.push_back(Vertex({x, height/2, z}, {0, 1, 0}, mesh.baseColor));
        // 底部顶点
        mesh.vertices.push_back(Vertex({x, -height/2, z}, {0, -1, 0}, mesh.baseColor));
        glm::vec3 sideNormal = glm::normalize(glm::vec3(x, 0.0f, z));
        // 侧面顶部顶点
        mesh.vertices.push_back(Vertex({x, height/2, z}, sideNormal, mesh.baseColor));
        // 侧面底部顶点
        mesh.vertices.push_back(Vertex({x, -height/2, z}, sideNormal, mesh.baseColor));
        
        // 顶部三角形
        if (i > 0) {
            mesh.indices.push_back(0); // 顶部中心
            mesh.indices.push_back(2 + (i-1)*4);
            mesh.indices.push_back(2 + i*4);
        }
        
        // 底部三角形
        if (i > 0) {
            mesh.indices.push_back(1); // 底部中心
            mesh.indices.push_back(3 + i*4);
            mesh.indices.push_back(3 + (i-1)*4);
        }
        
        // 侧面三角形
        if (i > 0) {
            int top1 = 4 + (i-1)*4;
            int top2 = 4 + i*4;
            int bottom1 = 5 + (i-1)*4;
            int bottom2 = 5 + i*4;
            
            mesh.indices.push_back(top1);
            mesh.indices.push_back(bottom1);
            mesh.indices.push_back(top2);
            
            mesh.indices.push_back(top2);
            mesh.indices.push_back(bottom1);
            mesh.indices.push_back(bottom2);
        }
    }
}

// 创建平面网格
void createPlaneMesh(Mesh& mesh, float size)
{
    mesh.vertices = {
        Vertex({-size/2, 0, -size/2}, {0, 1, 0}, mesh.baseColor),
        Vertex({size/2, 0, -size/2}, {0, 1, 0}, mesh.baseColor),
        Vertex({size/2, 0, size/2}, {0, 1, 0}, mesh.baseColor),
        Vertex({-size/2, 0, size/2}, {0, 1, 0}, mesh.baseColor)
    };
    
    mesh.indices = {
        0, 1, 2,
        2, 3, 0
    };
}

// 创建线段网格
void createLineMesh(Mesh& mesh, glm::vec3 start, glm::vec3 end)
{
    mesh.vertices = {
        Vertex(start, {0, 1, 0}, mesh.baseColor),
        Vertex(end, {0, 1, 0}, mesh.baseColor)
    };
    
    mesh.indices = {0, 1};
}

// 创建点网格
void createPointMesh(Mesh& mesh, glm::vec3 position)
{
    mesh.vertices = {
        Vertex(position, {0, 1, 0}, mesh.baseColor)
    };
    
    mesh.indices = {0};
}

// 创建平面网格（矩形）
void createPlaneMesh(Mesh& mesh, float width, float height)
{
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    
    mesh.vertices = {
        // 四个顶点
        Vertex({-halfWidth, 0.0f, -halfHeight}, {0, 1, 0}, mesh.baseColor), // 左后
        Vertex({halfWidth, 0.0f, -halfHeight}, {0, 1, 0}, mesh.baseColor),  // 右后
        Vertex({halfWidth, 0.0f, halfHeight}, {0, 1, 0}, mesh.baseColor),   // 右前
        Vertex({-halfWidth, 0.0f, halfHeight}, {0, 1, 0}, mesh.baseColor)    // 左前
    };
    
    // 使用两个三角形构成一个平面
    mesh.indices = {0, 1, 2, 2, 3, 0};
}

// 创建圆形面网格
void createCircleMesh(Mesh& mesh, float radius, int segments)
{
    // 中心点
    mesh.vertices.push_back(Vertex({0.0f, 0.0f, 0.0f}, {0, 1, 0}, mesh.baseColor));
    
    // 圆周上的点
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * glm::pi<float>() * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        
        mesh.vertices.push_back(Vertex({x, 0.0f, z}, {0, 1, 0}, mesh.baseColor));
    }
    
    // 构建三角扇形
    for (int i = 1; i <= segments; i++) {
        mesh.indices.push_back(0);        // 中心点
        mesh.indices.push_back(i);        // 当前点
        mesh.indices.push_back(i + 1);    // 下一个点
    }
    
    // 连接最后一个点到第一个点
    mesh.indices[segments * 3 - 1] = 1; // 替换最后一个索引为第一个点
}

bool loadModelFromFile(Mesh& mesh, const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.good()) {
        std::cout << "Model file not found: " << filename << std::endl;
        return false;
    }
    
    json modelJson;
    file >> modelJson;
    file.close();
    
    // 加载网格数据
    if (modelJson.contains("vertices")) {
        for (const auto& vertexData : modelJson["vertices"]) {
            Vertex vertex(
                {vertexData[0], vertexData[1], vertexData[2]},
                {vertexData[3], vertexData[4], vertexData[5]},
                {vertexData[6], vertexData[7], vertexData[8]}
            );
            mesh.vertices.push_back(vertex);
        }
    }
    
    if (modelJson.contains("indices")) {
        for (const auto& index : modelJson["indices"]) {
            mesh.indices.push_back(index);
        }
    }
    
    return true;
}

bool saveModelToFile(const Mesh& mesh, const std::string& filename)
{
    json modelJson;
    
    // 保存顶点
    for (const auto& vertex : mesh.vertices) {
        modelJson["vertices"].push_back({
            vertex.position.x, vertex.position.y, vertex.position.z,
            vertex.normal.x, vertex.normal.y, vertex.normal.z,
            vertex.color.x, vertex.color.y, vertex.color.z
        });
    }
    
    // 保存索引
    modelJson["indices"] = mesh.indices;
    
    // 确保目录存在
    fs::path path(filename);
    fs::path dir = path.parent_path();
    
    if (!dir.empty() && !fs::exists(dir)) {
        fs::create_directories(dir);
    }
    
    // 写入文件
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not open file for writing: " << filename << std::endl;
        return false;
    }
    file << modelJson.dump(4);
    file.close();
    
    return true;
}
