#include "transform_controller.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

TransformController::TransformController() 
    : isDragging(false), isScaling(false), isRotating(false), 
      startX(0), startY(0), 
      initialPosition(0.0f), initialScale(1.0f), initialRotation(0.0f) {}

void TransformController::startDrag(int x, int y, const Mesh& mesh) {
    isDragging = true;
    startX = x;
    startY = y;
    initialPosition = glm::vec3(mesh.transform[3]);
}

void TransformController::updateDrag(int x, int y, int windowWidth, int windowHeight, Mesh& mesh) {
    if (!isDragging) return;
    
    // 计算位移量
    float deltaX = (float)(x - startX) / windowWidth * 10.0f;  // 调整缩放系数
    float deltaY = (float)(startY - y) / windowHeight * 10.0f; // 注意Y轴翻转
    
    // 应用位移
    glm::vec3 newPosition = initialPosition + glm::vec3(deltaX, deltaY, 0.0f);
    
    // 更新变换矩阵的平移部分
    mesh.transform[3] = glm::vec4(newPosition, 1.0f);
}

void TransformController::endDrag() {
    isDragging = false;
    startX = 0;
    startY = 0;
}

void TransformController::startScale(int x, int y, const Mesh& mesh) {
    isScaling = true;
    startX = x;
    startY = y;
    
    // 获取当前缩放值（从变换矩阵的对角线元素获取）
    initialScale = glm::vec3(
        mesh.transform[0][0],
        mesh.transform[1][1],
        mesh.transform[2][2]
    );
}

void TransformController::updateScale(int x, int y, Mesh& mesh) {
    (void)x; // 未使用参数
    if (!isScaling) return;
    
    // 计算缩放增量
    float delta = (float)(y - startY) * 0.005f;  // 调整缩放灵敏度，减小缩放变化快慢参数
    
    // 计算新缩放值
    glm::vec3 newScale = initialScale + glm::vec3(delta, delta, delta);
    
    // 确保缩放值不会变为负数或零
    newScale.x = std::max(0.01f, newScale.x);
    newScale.y = std::max(0.01f, newScale.y);
    newScale.z = std::max(0.01f, newScale.z);
    
    // 重建变换矩阵，保留旋转信息但更新缩放
    glm::vec3 currentTranslation = glm::vec3(mesh.transform[3]);
    
    // 提取旋转部分（去除缩放影响）
    glm::mat3 rotationMatrix(
        glm::vec3(mesh.transform[0]) / initialScale.x,
        glm::vec3(mesh.transform[1]) / initialScale.y,
        glm::vec3(mesh.transform[2]) / initialScale.z
    );
    
    // 重新构建变换矩阵
    mesh.transform = glm::mat4(1.0f);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), newScale);
    glm::mat4 rotationMatrix4 = glm::mat4(rotationMatrix);
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), currentTranslation);
    
    mesh.transform = translationMatrix * rotationMatrix4 * scaleMatrix;
}

void TransformController::endScale() {
    isScaling = false;
    startX = 0;
    startY = 0;
}

void TransformController::startRotate(int x, int y, const Mesh& mesh) {
    (void)mesh; // 未使用参数
    isRotating = true;
    startX = x;
    startY = y;
    
    // 获取当前旋转（从变换矩阵提取）
    // 这里简化处理，实际应用中需要更复杂的矩阵分解
    initialRotation = glm::vec3(0.0f);
}

void TransformController::updateRotate(int x, int y, Mesh& mesh) {
    if (!isRotating) return;
    
    // 计算旋转增量
    float deltaX = (float)(x - startX) * 0.5f;  // 调整旋转灵敏度
    float deltaY = (float)(y - startY) * 0.5f;
    
    // 获取当前位置和当前缩放
    glm::vec3 currentTranslation = glm::vec3(mesh.transform[3]);
    glm::vec3 currentScale = glm::vec3(
        glm::length(glm::vec3(mesh.transform[0])),
        glm::length(glm::vec3(mesh.transform[1])),
        glm::length(glm::vec3(mesh.transform[2]))
    );
    
    // 重建变换矩阵
    mesh.transform = glm::mat4(1.0f);
    
    // 应用变换：先旋转，再缩放，最后平移
    glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(deltaY), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(deltaX), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotationMatrix = rotationY * rotationX;
    
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), currentScale);
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), currentTranslation);
    
    mesh.transform = translationMatrix * rotationMatrix * scaleMatrix;
}

void TransformController::endRotate() {
    isRotating = false;
    startX = 0;
    startY = 0;
}

bool TransformController::isTransforming() const {
    return isDragging || isScaling || isRotating;
}

void TransformController::setPosition(Mesh& mesh, glm::vec3 newPosition) {
    mesh.transform[3] = glm::vec4(newPosition, 1.0f);
}

void TransformController::setScale(Mesh& mesh, glm::vec3 newScale) {
    // 获取当前位置
    glm::vec3 currentTranslation = glm::vec3(mesh.transform[3]);
    
    // 提取当前旋转矩阵
    glm::vec3 currentScaleFactors = glm::vec3(
        glm::length(glm::vec3(mesh.transform[0])),
        glm::length(glm::vec3(mesh.transform[1])),
        glm::length(glm::vec3(mesh.transform[2]))
    );
    
    // 如果当前缩放因子为0，则使用默认值
    if (currentScaleFactors.x == 0) currentScaleFactors.x = 1.0f;
    if (currentScaleFactors.y == 0) currentScaleFactors.y = 1.0f;
    if (currentScaleFactors.z == 0) currentScaleFactors.z = 1.0f;
    
    // 计算旋转矩阵（去除缩放影响）
    glm::mat3 rotationMatrix(
        glm::vec3(mesh.transform[0]) / currentScaleFactors.x,
        glm::vec3(mesh.transform[1]) / currentScaleFactors.y,
        glm::vec3(mesh.transform[2]) / currentScaleFactors.z
    );
    
    // 重新构建变换矩阵
    mesh.transform = glm::mat4(1.0f);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), newScale);
    glm::mat4 rotationMatrix4 = glm::mat4(rotationMatrix);
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), currentTranslation);
    
    mesh.transform = translationMatrix * rotationMatrix4 * scaleMatrix;
}

void TransformController::setRotation(Mesh& mesh, glm::vec3 newRotation) {
    // 获取当前位置和缩放
    glm::vec3 currentTranslation = glm::vec3(mesh.transform[3]);
    glm::vec3 currentScale = glm::vec3(
        glm::length(glm::vec3(mesh.transform[0])),
        glm::length(glm::vec3(mesh.transform[1])),
        glm::length(glm::vec3(mesh.transform[2]))
    );
    
    // 应用新的旋转
    mesh.transform = glm::mat4(1.0f);
    
    glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(newRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(newRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(newRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 rotationMatrix = rotationZ * rotationY * rotationX;
    
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), currentScale);
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), currentTranslation);
    
    mesh.transform = translationMatrix * rotationMatrix * scaleMatrix;
}

glm::vec3 TransformController::getCurrentPosition(const Mesh& mesh) const {
    return glm::vec3(mesh.transform[3]);
}

glm::vec3 TransformController::getCurrentScale(const Mesh& mesh) const {
    return glm::vec3(
        glm::length(glm::vec3(mesh.transform[0])),
        glm::length(glm::vec3(mesh.transform[1])),
        glm::length(glm::vec3(mesh.transform[2]))
    );
}

glm::vec3 TransformController::getCurrentRotation(const Mesh& mesh) const {
    // 这是一个简化的实现，实际的旋转提取需要更复杂的矩阵分解
    // 返回近似值
    float rx = glm::degrees(asinf(-mesh.transform[1][2]));
    float ry = glm::degrees(atan2f(mesh.transform[0][2], mesh.transform[2][2]));
    float rz = glm::degrees(atan2f(mesh.transform[1][0], mesh.transform[1][1]));
    
    return glm::vec3(rx, ry, rz);
}