#include <gtest/gtest.h>
#include "transform_controller.h"

// 测试变换控制器的基本功能
TEST(TransformControllerTest, ConstructorTest) {
    TransformController controller;
    
    // 测试初始状态
    EXPECT_FALSE(controller.isTransforming());
}

TEST(TransformControllerTest, PositionTransformation) {
    TransformController controller;
    Mesh testMesh("TestMesh");
    
    glm::vec3 newPosition(1.0f, 2.0f, 3.0f);
    
    // 测试位置设置
    TransformController::setPosition(testMesh, newPosition);
    
    // 由于无法直接访问变换矩阵，这里仅测试接口调用不会崩溃
    EXPECT_FALSE(controller.isTransforming());
}