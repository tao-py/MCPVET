#include <gtest/gtest.h>
#include "render.h"

// 测试渲染模块的基本功能
TEST(RenderTest, ShaderSetup) {
    // 由于渲染模块涉及OpenGL上下文，这里只测试是否存在必要的函数
    // 实际渲染测试需要有效的OpenGL上下文，因此这里主要是接口测试
    
    // 验证着色器源码存在
    EXPECT_NE(vertexShaderSource, nullptr);
    EXPECT_NE(fragmentShaderSource, nullptr);
}

// 更多渲染相关的测试将在具备OpenGL上下文的环境中进行