#include "scene_manager.h"
#include "config_manager.h"  // 现在包含了全局变量声明
#include <filesystem>
#include <iostream>
#include <limits>
#include <manifold/manifold.h>

namespace fs = std::filesystem;

// 新增：建模文件路径
const std::string CUBE_MODEL_FILE = "models/cube.model";
const std::string SPHERE_MODEL_FILE = "models/sphere.model";
const std::string CYLINDER_MODEL_FILE = "models/cylinder.model";
const std::string PLANE_MODEL_FILE = "models/plane.model";

void ensureSceneDirectory(const std::string& filepath) {
    fs::path path(filepath);
    fs::path dir = path.parent_path();
    
    if (!dir.empty() && !fs::exists(dir)) {
        fs::create_directories(dir);
    }
}

void saveScene(const std::string& filename)
{
    // 确保目录存在
    ensureSceneDirectory(filename);
    
    json scene;
    
    // 保存场景设置
    scene["scene_settings"]["camera_distance"] = sceneState.cameraDistance;
    scene["scene_settings"]["camera_angles"] = {sceneState.cameraAngles.x, sceneState.cameraAngles.y};
    
    // 保存所有网格
    for (int i = 0; i < meshes.size(); i++) {
        json meshJson;
        meshJson["name"] = meshes[i].name;
        
        // 保存变换矩阵
        std::vector<float> transformData;
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                transformData.push_back(meshes[i].transform[col][row]);
            }
        }
        meshJson["transform"] = transformData;
        
        // 保存颜色
        meshJson["color"] = {meshes[i].baseColor.r, meshes[i].baseColor.g, meshes[i].baseColor.b};
        
        // 保存顶点
        for (const auto& vertex : meshes[i].vertices) {
            meshJson["vertices"].push_back({
                vertex.position.x, vertex.position.y, vertex.position.z,
                vertex.normal.x, vertex.normal.y, vertex.normal.z,
                vertex.color.x, vertex.color.y, vertex.color.z
            });
        }
        
        // 保存索引
        meshJson["indices"] = meshes[i].indices;
        
        scene["meshes"].push_back(meshJson);
    }
    
    // 写入文件
    std::ofstream file(filename);
    file << scene.dump(4);
    file.close();
}

void loadScene(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.good()) {
        std::cout << "Scene file not found: " << filename << std::endl;
        return;
    }
    
    json scene;
    file >> scene;
    file.close();
    
    // 清空当前场景
    meshes.clear();
    originalMeshes.clear();
    selectedMesh = -1;
    secondMeshForBoolean = -1;
    
    // 加载场景设置
    if (scene.contains("scene_settings")) {
        if (scene["scene_settings"].contains("camera_distance"))
            sceneState.cameraDistance = scene["scene_settings"]["camera_distance"];
        if (scene["scene_settings"].contains("camera_angles")) {
            sceneState.cameraAngles.x = scene["scene_settings"]["camera_angles"][0];
            sceneState.cameraAngles.y = scene["scene_settings"]["camera_angles"][1];
        }
    }
    
    // 加载所有网格
    if (scene.contains("meshes")) {
        for (const auto& meshJson : scene["meshes"]) {
            Mesh mesh;
            mesh.name = meshJson["name"];
            
            // 加载变换矩阵
            if (meshJson.contains("transform")) {
                auto transformData = meshJson["transform"];
                for (int row = 0; row < 4; row++) {
                    for (int col = 0; col < 4; col++) {
                        mesh.transform[col][row] = transformData[row * 4 + col];
                    }
                }
            }
            
            // 加载颜色
            if (meshJson.contains("color")) {
                mesh.baseColor.r = meshJson["color"][0];
                mesh.baseColor.g = meshJson["color"][1];
                mesh.baseColor.b = meshJson["color"][2];
            }
            
            // 加载顶点
            if (meshJson.contains("vertices")) {
                for (const auto& vertexData : meshJson["vertices"]) {
                    Vertex vertex(
                        {vertexData[0], vertexData[1], vertexData[2]},
                        {vertexData[3], vertexData[4], vertexData[5]},
                        {vertexData[6], vertexData[7], vertexData[8]}
                    );
                    mesh.vertices.push_back(vertex);
                }
            }
            
            // 加载索引
            if (meshJson.contains("indices")) {
                for (const auto& index : meshJson["indices"]) {
                    mesh.indices.push_back(index);
                }
            }
            
            meshes.push_back(mesh);
            originalMeshes.push_back(mesh);
        }
    }
}

namespace {

bool isTriangleMesh(const Mesh& mesh) {
    return !mesh.vertices.empty() && (mesh.indices.size() >= 3) && (mesh.indices.size() % 3 == 0);
}

bool meshToManifold(const Mesh& mesh, manifold::Manifold& outManifold, std::string& error) {
    if (!isTriangleMesh(mesh)) {
        error = "Mesh is not a triangle mesh.";
        return false;
    }

    manifold::MeshGL meshGL;
    meshGL.numProp = 3;
    meshGL.vertProperties.reserve(mesh.vertices.size() * 3);
    for (const auto& vertex : mesh.vertices) {
        glm::vec4 worldPos = mesh.transform * glm::vec4(vertex.position, 1.0f);
        meshGL.vertProperties.push_back(worldPos.x);
        meshGL.vertProperties.push_back(worldPos.y);
        meshGL.vertProperties.push_back(worldPos.z);
    }

    meshGL.triVerts.reserve(mesh.indices.size());
    for (const auto& index : mesh.indices) {
        meshGL.triVerts.push_back(static_cast<uint32_t>(index));
    }

    manifold::Manifold manifold(meshGL);
    if (manifold.Status() != manifold::Manifold::Error::NoError) {
        error = "Manifold construction failed.";
        return false;
    }
    outManifold = std::move(manifold);
    return true;
}

void computeNormals(const std::vector<glm::vec3>& positions,
                    const std::vector<uint32_t>& indices,
                    std::vector<glm::vec3>& normals) {
    normals.assign(positions.size(), glm::vec3(0.0f));
    for (size_t i = 0; i + 2 < indices.size(); i += 3) {
        uint32_t i0 = indices[i];
        uint32_t i1 = indices[i + 1];
        uint32_t i2 = indices[i + 2];
        if (i0 >= positions.size() || i1 >= positions.size() || i2 >= positions.size()) {
            continue;
        }
        const glm::vec3& v0 = positions[i0];
        const glm::vec3& v1 = positions[i1];
        const glm::vec3& v2 = positions[i2];
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        normals[i0] += normal;
        normals[i1] += normal;
        normals[i2] += normal;
    }
    for (auto& n : normals) {
        if (glm::length(n) > 0.0f) {
            n = glm::normalize(n);
        } else {
            n = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }
}

} // namespace

void performBooleanOperation(BooleanOperation operation)
{
    if (selectedMesh < 0 || secondMeshForBoolean < 0 || 
        selectedMesh >= meshes.size() || secondMeshForBoolean >= meshes.size()) {
        std::cout << "Please select two objects for boolean operation" << std::endl;
        return;
    }
    
    // 简化的布尔运算实现
    // 实际应用中应该使用更复杂的算法，如CSG或BSP树
    Mesh result;
    switch (operation) {
        case BooleanOperation::UNION:
            result.name = "BooleanUnion";
            break;
        case BooleanOperation::DIFFERENCE:
            result.name = "BooleanDifference";
            break;
        case BooleanOperation::INTERSECTION:
            result.name = "BooleanIntersection";
            break;
    }
    
    // 使用 Manifold 实现真实布尔运算（要求三角网格）
    const Mesh& mesh1 = originalMeshes[selectedMesh];
    const Mesh& mesh2 = originalMeshes[secondMeshForBoolean];

    manifold::Manifold a;
    manifold::Manifold b;
    std::string error;
    if (!meshToManifold(mesh1, a, error) || !meshToManifold(mesh2, b, error)) {
        std::cout << "Boolean operation failed: " << error << std::endl;
        return;
    }

    manifold::OpType opType = manifold::OpType::Add;
    if (operation == BooleanOperation::DIFFERENCE) {
        opType = manifold::OpType::Subtract;
    } else if (operation == BooleanOperation::INTERSECTION) {
        opType = manifold::OpType::Intersect;
    }

    manifold::Manifold output = a.Boolean(b, opType);
    if (output.Status() != manifold::Manifold::Error::NoError || output.IsEmpty()) {
        std::cout << "Boolean operation produced no result." << std::endl;
        return;
    }

    manifold::MeshGL meshGL = output.GetMeshGL();
    const size_t vertCount = meshGL.NumVert();
    if (vertCount == 0 || meshGL.triVerts.empty()) {
        std::cout << "Boolean operation produced empty mesh." << std::endl;
        return;
    }

    std::vector<glm::vec3> positions;
    positions.reserve(vertCount);
    for (size_t i = 0; i < vertCount; ++i) {
        size_t base = i * meshGL.numProp;
        positions.emplace_back(meshGL.vertProperties[base],
                               meshGL.vertProperties[base + 1],
                               meshGL.vertProperties[base + 2]);
    }

    std::vector<glm::vec3> normals;
    computeNormals(positions, meshGL.triVerts, normals);

    result.vertices.clear();
    result.indices.clear();
    result.vertices.reserve(positions.size());
    result.indices.reserve(meshGL.triVerts.size());

    glm::vec3 baseColor = mesh1.baseColor;
    if (operation == BooleanOperation::INTERSECTION) {
        baseColor = mesh2.baseColor;
    }
    result.baseColor = baseColor;

    for (size_t i = 0; i < positions.size(); ++i) {
        result.vertices.emplace_back(positions[i], normals[i], baseColor);
    }
    for (const auto& index : meshGL.triVerts) {
        result.indices.push_back(index);
    }

    result.transform = glm::mat4(1.0f);
    
    // 添加到场景
    meshes.push_back(result);
    originalMeshes.push_back(result);
    selectedMesh = meshes.size() - 1;
    secondMeshForBoolean = -1;
    
    std::cout << "Boolean operation completed" << std::endl;
}

// 射线与三角形相交检测（使用Möller-Trumbore算法）
bool rayIntersectsTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                         const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                         float& distance) {
    const float EPSILON = 1e-8;
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(rayDir, edge2);
    float a = glm::dot(edge1, h);
    
    if (a > -EPSILON && a < EPSILON) return false;
    
    float f = 1.0 / a;
    glm::vec3 s = rayOrigin - v0;
    float u = f * glm::dot(s, h);
    
    if (u < 0.0 || u > 1.0) return false;
    
    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(rayDir, q);
    
    if (v < 0.0 || u + v > 1.0) return false;
    
    float t = f * glm::dot(edge2, q);
    if (t > EPSILON) {
        distance = t;
        return true;
    }
    
    return false;
}

// 选择对象
int pickObject(double mouseX, double mouseY, int windowWidth, int windowHeight) {
    // 将屏幕坐标转换为标准化设备坐标(-1到1)
    float ndcX = (2.0f * mouseX) / windowWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * mouseY) / windowHeight;
    
    // 获取视图和投影矩阵
    glm::mat4 view = sceneState.viewMatrix;
    glm::mat4 proj = sceneState.projectionMatrix;
    
    // 构建VP矩阵的逆矩阵
    glm::mat4 invVP = glm::inverse(proj * view);
    
    // 创建近平面和远平面上的点
    glm::vec4 nearPoint = invVP * glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
    glm::vec4 farPoint = invVP * glm::vec4(ndcX, ndcY, 1.0f, 1.0f);
    
    // 归一化齐次坐标
    nearPoint /= nearPoint.w;
    farPoint /= farPoint.w;
    
    // 射线方向
    glm::vec3 rayOrigin = glm::vec3(nearPoint);
    glm::vec3 rayDir = glm::normalize(glm::vec3(farPoint) - rayOrigin);
    
    // 遍历所有网格，寻找最近的相交点
    int closestMesh = -1;
    float closestDistance = std::numeric_limits<float>::max();
    
    for (int i = 0; i < meshes.size(); i++) {
        const Mesh& mesh = meshes[i];
        
        // 对每个三角形进行射线相交检测
        for (size_t j = 0; j < mesh.indices.size(); j += 3) {
            if (j + 2 >= mesh.indices.size()) continue;
            
            // 获取三角形顶点
            glm::vec3 v0 = glm::vec3(mesh.transform * glm::vec4(mesh.vertices[mesh.indices[j]].position, 1.0f));
            glm::vec3 v1 = glm::vec3(mesh.transform * glm::vec4(mesh.vertices[mesh.indices[j+1]].position, 1.0f));
            glm::vec3 v2 = glm::vec3(mesh.transform * glm::vec4(mesh.vertices[mesh.indices[j+2]].position, 1.0f));
            
            float distance;
            if (rayIntersectsTriangle(rayOrigin, rayDir, v0, v1, v2, distance)) {
                if (distance > 0 && distance < closestDistance) {
                    closestDistance = distance;
                    closestMesh = i;
                }
            }
        }
    }
    
    return closestMesh;
}
