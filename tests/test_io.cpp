#include <gtest/gtest.h>
#include "config_manager.h"
#include "command_parser.h"
#include <string>

// 测试命令解析功能
TEST(CommandParserTest, ParseValidCommand) {
    CommandParser parser;
    std::vector<Mesh> meshes;
    
    // 测试简单的球体创建命令
    bool result = parser.parseAndExecute("sphere(5)", meshes);
    
    // 根据实际情况调整期望值
    EXPECT_TRUE(result);
    EXPECT_EQ(meshes.size(), 1);
    EXPECT_EQ(meshes.front().name, "Sphere");
}

TEST(ConfigManagerTest, LoadDefaultConfig) {
    // 测试配置加载功能
    SceneState originalState = sceneState;
    
    // 加载默认配置
    loadConfig();
    
    // 验证某些默认值
    EXPECT_TRUE(sceneState.showGrid);
    EXPECT_EQ(sceneState.gridColor, glm::vec3(0.5f, 0.5f, 0.5f));
    
    // 恢复原始状态
    sceneState = originalState;
}
