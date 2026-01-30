#ifndef TRANSFORM_CONTROLLER_H
#define TRANSFORM_CONTROLLER_H

#include "vertex_mesh.h"
#include <GLFW/glfw3.h>

class TransformController {
private:
    bool isDragging;
    bool isScaling;
    bool isRotating;
    int startX, startY;
    glm::vec3 initialPosition;
    glm::vec3 initialScale;
    glm::vec3 initialRotation;

public:
    TransformController();
    
    // 开始拖拽操作
    void startDrag(int x, int y, const Mesh& mesh);
    
    // 更新拖拽操作
    void updateDrag(int x, int y, int windowWidth, int windowHeight, Mesh& mesh);
    
    // 结束拖拽操作
    void endDrag();
    
    // 开始缩放操作
    void startScale(int x, int y, const Mesh& mesh);
    
    // 更新缩放操作
    void updateScale(int x, int y, Mesh& mesh);
    
    // 结束缩放操作
    void endScale();
    
    // 开始旋转操作
    void startRotate(int x, int y, const Mesh& mesh);
    
    // 更新旋转操作
    void updateRotate(int x, int y, Mesh& mesh);
    
    // 结束旋转操作
    void endRotate();
    
    // 检查是否正在执行某种变换
    bool isTransforming() const;
    
    // 应用变换到网格
    static void setPosition(Mesh& mesh, glm::vec3 newPosition);
    static void setScale(Mesh& mesh, glm::vec3 newScale);
    static void setRotation(Mesh& mesh, glm::vec3 newRotation);
    
    // 获取当前变换值
    glm::vec3 getCurrentPosition(const Mesh& mesh) const;
    glm::vec3 getCurrentScale(const Mesh& mesh) const;
    glm::vec3 getCurrentRotation(const Mesh& mesh) const;
};

#endif // TRANSFORM_CONTROLLER_H