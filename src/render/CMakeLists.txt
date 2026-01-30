# Render module library
add_library(render
    render.cpp
    Framebuffer.h
    Framebuffer.cpp
)

# 导出接口包含目录
target_include_directories(render
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/imgui
)

# 链接依赖库
target_link_libraries(render
    PUBLIC
        project_options
        project_warnings
        core
        dearimgui
        glad
        glfw
        OpenGL::GL
        glm::glm
)
