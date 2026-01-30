# Third-party dependencies management

include(FetchContent)

# OpenGL
find_package(OpenGL REQUIRED)

# GLAD - 使用本地版本
add_library(glad STATIC
    ${CMAKE_SOURCE_DIR}/include/glad/glad.c
)

target_include_directories(glad
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(glad
    PUBLIC
        OpenGL::GL
)

# GLFW - 尝试查找系统安装版本，否则使用内置版本
find_package(glfw3 QUIET)
if(glfw3_FOUND)
    if(TARGET glfw)
        # Use provided target name as-is.
    elseif(TARGET glfw3)
        add_library(glfw ALIAS glfw3)
    elseif(TARGET GLFW::GLFW)
        add_library(glfw ALIAS GLFW::GLFW)
    else()
        message(FATAL_ERROR "glfw3 found but no usable CMake target was exported")
    endif()
else()
    message(WARNING "System GLFW not found, using bundled version")
    # 下载并构建GLFW库
    include(FetchContent)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8
    )
    FetchContent_MakeAvailable(glfw)
    
    # 设置GLFW别名
    if(NOT TARGET glfw AND TARGET glfw3)
        add_library(glfw ALIAS glfw3)
    endif()
endif()

# Dear ImGui
set(DEARIMGUI_LOCAL_DIR "${CMAKE_SOURCE_DIR}/include/imgui")
if(EXISTS "${DEARIMGUI_LOCAL_DIR}/imgui.cpp")
    set(dearimgui_SOURCE_DIR "${DEARIMGUI_LOCAL_DIR}")
else()
    FetchContent_Declare(
        dearimgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.91
    )
    FetchContent_MakeAvailable(dearimgui)
endif()

# 创建ImGui库，但仅包含一次
add_library(dearimgui STATIC
    ${dearimgui_SOURCE_DIR}/imgui.cpp
    ${dearimgui_SOURCE_DIR}/imgui_draw.cpp
    ${dearimgui_SOURCE_DIR}/imgui_tables.cpp
    ${dearimgui_SOURCE_DIR}/imgui_widgets.cpp
    ${dearimgui_SOURCE_DIR}/imgui_demo.cpp
    ${dearimgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${dearimgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(mcnp::imgui ALIAS dearimgui)

target_include_directories(dearimgui
    PUBLIC
        ${dearimgui_SOURCE_DIR}
        ${dearimgui_SOURCE_DIR}/backends
)

target_link_libraries(dearimgui
    PUBLIC
        OpenGL::GL
)

# 仅在需要的地方链接GLFW
target_link_libraries(dearimgui
    PRIVATE
        glfw
)

# GLM
set(GLM_LOCAL_DIR "${CMAKE_SOURCE_DIR}/include/glm")
if(EXISTS "${GLM_LOCAL_DIR}/glm/glm.hpp")
    add_library(glm::glm INTERFACE IMPORTED)
    target_include_directories(glm::glm
        INTERFACE
            ${GLM_LOCAL_DIR}
    )
else()
    find_package(glm REQUIRED)
endif()

# nlohmann/json
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    message(WARNING "nlohmann_json not found, using bundled version")
    add_library(nlohmann_json::nlohmann_json INTERFACE IMPORTED)
    target_include_directories(nlohmann_json::nlohmann_json
        INTERFACE
            ${CMAKE_SOURCE_DIR}/include/nlohmann
    )
endif()

# Manifold (mesh boolean)
# 使用本地已下载的Manifold库
set(FETCHCONTENT_SOURCE_DIR_MANIFOLD "${CMAKE_SOURCE_DIR}/include/manifold" CACHE STRING "")
set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE BOOL "" FORCE)

set(MANIFOLD_TEST OFF CACHE BOOL "" FORCE)
set(MANIFOLD_EXPORT OFF CACHE BOOL "" FORCE)
set(MANIFOLD_PAR OFF CACHE BOOL "" FORCE)
set(MANIFOLD_CROSS_SECTION OFF CACHE BOOL "" FORCE)
set(MANIFOLD_DOWNLOADS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    manifold
    GIT_REPOSITORY https://github.com/elalish/manifold.git
    GIT_TAG v3.3.2
)

# 配置Manifold库的编译选项，特别是对于macOS系统
if(APPLE)
    set(MANIFOLD_CXX_STANDARD 17 CACHE STRING "" FORCE)
    set(MANIFOLD_CXX_EXTENSIONS OFF CACHE BOOL "" FORCE)
    set(MANIFOLD_CXX_STANDARD_REQUIRED ON CACHE BOOL "" FORCE)

endif()

FetchContent_MakeAvailable(manifold)

# 为macOS设置标准库路径
if(APPLE)
    if(TARGET manifold)
        target_compile_options(manifold PRIVATE
            $<$<CXX_COMPILER_ID:AppleClang>:-I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/c++/v1>
        )
        target_include_directories(manifold PRIVATE
            /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/c++/v1
        )
    endif()
endif()

if(TARGET manifold AND NOT TARGET manifold::manifold)
    add_library(manifold::manifold ALIAS manifold)
endif()

message(STATUS "Using local Manifold library from: ${CMAKE_SOURCE_DIR}/include/manifold")