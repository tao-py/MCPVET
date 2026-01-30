#include <gtest/gtest.h>
#include "vertex_mesh.h"
#include "geometry_factory.h"

// 测试Mesh类的基本功能
TEST(MeshTest, ConstructorTest) {
    Mesh mesh("TestMesh");
    EXPECT_EQ(mesh.name, "TestMesh");
    EXPECT_TRUE(mesh.vertices.empty());
    EXPECT_TRUE(mesh.indices.empty());
}

// 测试几何体创建功能
TEST(GeometryFactoryTest, CreateSphere) {
    Mesh mesh;
    GeometryFactory::createSphere(mesh, 1.0f, 8); // 使用较小的细分以加快测试
    
    // 检查是否创建了顶点
    EXPECT_FALSE(mesh.vertices.empty());
    EXPECT_FALSE(mesh.indices.empty());
    
    EXPECT_EQ(mesh.name, "Sphere");
}

TEST(GeometryFactoryTest, CreateBox) {
    Mesh mesh;
    GeometryFactory::createBox(mesh, 1.0f, 1.0f, 1.0f);
    
    EXPECT_FALSE(mesh.vertices.empty());
    EXPECT_FALSE(mesh.indices.empty());
    
    EXPECT_EQ(mesh.name, "Box");
}
