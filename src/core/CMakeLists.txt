# Core module library
add_library(core
    camera.cpp
    vertex_mesh.cpp
    geometry_factory.cpp
    geometry_model.cpp
    log_manager.cpp
    coordinate_system.cpp
    ../path/savepath.cpp
)

# 导出接口包含目录
target_include_directories(core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../path
)

# 链接依赖库
target_link_libraries(core
    PUBLIC
        project_options
        project_warnings
        glad
        OpenGL::GL
        glm::glm
)
