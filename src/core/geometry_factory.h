#ifndef GEOMETRY_FACTORY_H
#define GEOMETRY_FACTORY_H

#include "vertex_mesh.h"
#include <string>

class GeometryFactory {
public:
    // 创建点
    static void createPoint(Mesh& mesh, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
    
    // 创建线段
    static void createLine(Mesh& mesh, glm::vec3 start = glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec3 end = glm::vec3(0.5f, 0.0f, 0.0f));
    
    // 创建矩形平面
    static void createRectangle(Mesh& mesh, float width = 1.0f, float height = 1.0f);
    
    // 创建圆形平面
    static void createCircle(Mesh& mesh, float radius = 0.5f, int segments = 32);
    
    // 创建球体
    static void createSphere(Mesh& mesh, float radius = 0.5f, int segments = 32);
    
    // 创建立方体/长方体
    static void createBox(Mesh& mesh, float width = 1.0f, float height = 1.0f, float depth = 1.0f);
    
    // 创建圆柱体
    static void createCylinder(Mesh& mesh, float radius = 0.5f, float height = 1.0f, int segments = 32);
    
    // 从字符串命令创建几何体
    static bool createFromCommand(Mesh& mesh, const std::string& command);
};

#endif // GEOMETRY_FACTORY_H