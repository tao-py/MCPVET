#include "geometry_factory.h"
#include <sstream>
#include <algorithm>
#include <cctype>

void GeometryFactory::createPoint(Mesh& mesh, glm::vec3 position) {
    createPointMesh(mesh, position);
    mesh.name = "Point";
}

void GeometryFactory::createLine(Mesh& mesh, glm::vec3 start, glm::vec3 end) {
    createLineMesh(mesh, start, end);
    mesh.name = "Line";
}

void GeometryFactory::createRectangle(Mesh& mesh, float width, float height) {
    createPlaneMesh(mesh, width, height);
    mesh.name = "Rectangle";
}

void GeometryFactory::createCircle(Mesh& mesh, float radius, int segments) {
    createCircleMesh(mesh, radius, segments);
    mesh.name = "Circle";
}

void GeometryFactory::createSphere(Mesh& mesh, float radius, int segments) {
    // 重新实现球体创建以使用半径参数
    mesh.vertices.clear();
    mesh.indices.clear();
    
    for (int i = 0; i <= segments; i++) {
        float lat0 = glm::pi<float>() * (-0.5f + (float)(i - 1) / segments);
        float z0 = sin(lat0);
        float zr0 = cos(lat0);
        
        float lat1 = glm::pi<float>() * (-0.5f + (float)i / segments);
        float z1 = sin(lat1);
        float zr1 = cos(lat1);
        
        for (int j = 0; j <= segments; j++) {
            float lng = 2 * glm::pi<float>() * (float)(j - 1) / segments;
            float x = cos(lng);
            float y = sin(lng);
            
            glm::vec3 pos0 = glm::vec3(x * zr0 * radius, y * zr0 * radius, z0 * radius);
            glm::vec3 pos1 = glm::vec3(x * zr1 * radius, y * zr1 * radius, z1 * radius);
            
            mesh.vertices.push_back(Vertex(pos0, glm::normalize(pos0), mesh.baseColor));
            mesh.vertices.push_back(Vertex(pos1, glm::normalize(pos1), mesh.baseColor));
            
            if (i > 0 && j > 0) {
                int a = (i * (segments + 1) + j) * 2;
                int b = a - 1;
                int c = a - 2;
                int d = b - 2;
                
                mesh.indices.push_back(a);
                mesh.indices.push_back(b);
                mesh.indices.push_back(c);
                
                mesh.indices.push_back(c);
                mesh.indices.push_back(b);
                mesh.indices.push_back(d);
            }
        }
    }
    mesh.name = "Sphere";
}

void GeometryFactory::createBox(Mesh& mesh, float width, float height, float depth) {
    // 重新实现长方体创建以使用自定义尺寸
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    float halfDepth = depth / 2.0f;
    
    mesh.vertices = {
        // 前面
        Vertex({-halfWidth, -halfHeight, halfDepth}, {0.0f, 0.0f, 1.0f}, mesh.baseColor),
        Vertex({halfWidth, -halfHeight, halfDepth}, {0.0f, 0.0f, 1.0f}, mesh.baseColor),
        Vertex({halfWidth, halfHeight, halfDepth}, {0.0f, 0.0f, 1.0f}, mesh.baseColor),
        Vertex({-halfWidth, halfHeight, halfDepth}, {0.0f, 0.0f, 1.0f}, mesh.baseColor),
        
        // 后面
        Vertex({-halfWidth, -halfHeight, -halfDepth}, {0.0f, 0.0f, -1.0f}, mesh.baseColor),
        Vertex({halfWidth, -halfHeight, -halfDepth}, {0.0f, 0.0f, -1.0f}, mesh.baseColor),
        Vertex({halfWidth, halfHeight, -halfDepth}, {0.0f, 0.0f, -1.0f}, mesh.baseColor),
        Vertex({-halfWidth, halfHeight, -halfDepth}, {0.0f, 0.0f, -1.0f}, mesh.baseColor),
        
        // 左面
        Vertex({-halfWidth, -halfHeight, -halfDepth}, {-1.0f, 0.0f, 0.0f}, mesh.baseColor),
        Vertex({-halfWidth, -halfHeight, halfDepth}, {-1.0f, 0.0f, 0.0f}, mesh.baseColor),
        Vertex({-halfWidth, halfHeight, halfDepth}, {-1.0f, 0.0f, 0.0f}, mesh.baseColor),
        Vertex({-halfWidth, halfHeight, -halfDepth}, {-1.0f, 0.0f, 0.0f}, mesh.baseColor),
        
        // 右面
        Vertex({halfWidth, -halfHeight, -halfDepth}, {1.0f, 0.0f, 0.0f}, mesh.baseColor),
        Vertex({halfWidth, -halfHeight, halfDepth}, {1.0f, 0.0f, 0.0f}, mesh.baseColor),
        Vertex({halfWidth, halfHeight, halfDepth}, {1.0f, 0.0f, 0.0f}, mesh.baseColor),
        Vertex({halfWidth, halfHeight, -halfDepth}, {1.0f, 0.0f, 0.0f}, mesh.baseColor),
        
        // 上面
        Vertex({-halfWidth, halfHeight, -halfDepth}, {0.0f, 1.0f, 0.0f}, mesh.baseColor),
        Vertex({halfWidth, halfHeight, -halfDepth}, {0.0f, 1.0f, 0.0f}, mesh.baseColor),
        Vertex({halfWidth, halfHeight, halfDepth}, {0.0f, 1.0f, 0.0f}, mesh.baseColor),
        Vertex({-halfWidth, halfHeight, halfDepth}, {0.0f, 1.0f, 0.0f}, mesh.baseColor),
        
        // 下面
        Vertex({-halfWidth, -halfHeight, -halfDepth}, {0.0f, -1.0f, 0.0f}, mesh.baseColor),
        Vertex({halfWidth, -halfHeight, -halfDepth}, {0.0f, -1.0f, 0.0f}, mesh.baseColor),
        Vertex({halfWidth, -halfHeight, halfDepth}, {0.0f, -1.0f, 0.0f}, mesh.baseColor),
        Vertex({-halfWidth, -halfHeight, halfDepth}, {0.0f, -1.0f, 0.0f}, mesh.baseColor)
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
    mesh.name = "Box";
}

void GeometryFactory::createCylinder(Mesh& mesh, float radius, float height, int segments) {
    // 重新实现圆柱体创建以使用自定义参数
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
    mesh.name = "Cylinder";
}

bool GeometryFactory::createFromCommand(Mesh& mesh, const std::string& command) {
    std::string cmd = command;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    
    // 移除所有空格
    cmd.erase(std::remove_if(cmd.begin(), cmd.end(), ::isspace), cmd.end());
    
    if (cmd.substr(0, 5) == "point") {
        // 解析 Point(x,y,z) 格式
        size_t start = cmd.find('(');
        size_t end = cmd.find(')');
        if (start != std::string::npos && end != std::string::npos) {
            std::string params = cmd.substr(start + 1, end - start - 1);
            std::stringstream ss(params);
            std::string item;
            std::vector<float> values;
            
            while (std::getline(ss, item, ',')) {
                try {
                    values.push_back(std::stof(item));
                } catch (...) {
                    return false;
                }
            }
            
            if (values.size() == 3) {
                createPoint(mesh, glm::vec3(values[0], values[1], values[2]));
                return true;
            }
        }
    } else if (cmd.substr(0, 4) == "line") {
        // 解析 Line(x1,y1,z1,x2,y2,z2) 格式
        size_t start = cmd.find('(');
        size_t end = cmd.find(')');
        if (start != std::string::npos && end != std::string::npos) {
            std::string params = cmd.substr(start + 1, end - start - 1);
            std::stringstream ss(params);
            std::string item;
            std::vector<float> values;
            
            while (std::getline(ss, item, ',')) {
                try {
                    values.push_back(std::stof(item));
                } catch (...) {
                    return false;
                }
            }
            
            if (values.size() == 6) {
                createLine(mesh, glm::vec3(values[0], values[1], values[2]), 
                                 glm::vec3(values[3], values[4], values[5]));
                return true;
            }
        }
    } else if (cmd.substr(0, 5) == "plane" || cmd.substr(0, 9) == "rectangle") {
        // 解析 Plane(width,height) 或 Rectangle(width,height) 格式
        size_t start = cmd.find('(');
        size_t end = cmd.find(')');
        if (start != std::string::npos && end != std::string::npos) {
            std::string params = cmd.substr(start + 1, end - start - 1);
            std::stringstream ss(params);
            std::string item;
            std::vector<float> values;
            
            while (std::getline(ss, item, ',')) {
                try {
                    values.push_back(std::stof(item));
                } catch (...) {
                    return false;
                }
            }
            
            if (values.size() == 2) {
                createRectangle(mesh, values[0], values[1]);
                return true;
            } else if (values.size() == 1) {
                createRectangle(mesh, values[0], values[0]);
                return true;
            }
        }
    } else if (cmd.substr(0, 6) == "circle") {
        // 解析 Circle(radius) 格式
        size_t start = cmd.find('(');
        size_t end = cmd.find(')');
        if (start != std::string::npos && end != std::string::npos) {
            std::string params = cmd.substr(start + 1, end - start - 1);
            try {
                float radius = std::stof(params);
                createCircle(mesh, radius);
                return true;
            } catch (...) {
                return false;
            }
        }
    }
    
    return false;
}
