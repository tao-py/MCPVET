# IO module library
add_library(io
    config_manager.cpp
    scene_manager.cpp
    command_parser.cpp
    input_ast.cpp
    mcnp_parser.cpp
)

# 导出接口包含目录
target_include_directories(io
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接依赖库
target_link_libraries(io
    PUBLIC
        project_options
        project_warnings
        core
        dearimgui
        glad
        glfw
        nlohmann_json::nlohmann_json
        manifold::manifold
    PRIVATE
        OpenGL::GL
)
