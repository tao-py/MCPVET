cmake_minimum_required(VERSION 3.15)

project(NCPVET
    VERSION 0.1.0
    LANGUAGES CXX C
)

option(MCNP_ENABLE_IMGUI_DOCKING "Enable ImGui docking (requires docking branch)" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 统一依赖管理，在 add_subdirectory 之前
include(cmake/dependencies.cmake)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)
target_compile_definitions(project_options INTERFACE IMGUI_DEFINE_MATH_OPERATORS)
if(MCNP_ENABLE_IMGUI_DOCKING)
    target_compile_definitions(project_options INTERFACE IMGUI_HAS_DOCK MCNP_IMGUI_DOCKING)
endif()

add_library(project_warnings INTERFACE)
target_compile_options(project_warnings INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
)

if(APPLE)
    set(CXX_STDLIB_INCLUDE "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/c++/v1")
    if(EXISTS "${CXX_STDLIB_INCLUDE}/iostream")
        target_include_directories(project_options INTERFACE "${CXX_STDLIB_INCLUDE}")
        # Also set global include for subprojects like Manifold
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${CXX_STDLIB_INCLUDE}")
    endif()
endif()

add_subdirectory(src)

# 对应可执行文件
add_executable(MCPVET
    src/main.cpp
    ${CMAKE_SOURCE_DIR}/include/imgui/ImGuiFileDialog.cpp
)

# 统一输出目录，确保资源路径如models/ 正确
set_target_properties(MCPVET PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/bin"
)
if(MSVC)
    set_target_properties(MCPVET PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:MCPVET>"
    )
endif()
if(XCODE)
    set_target_properties(MCPVET PROPERTIES
        XCODE_SCHEME_WORKING_DIRECTORY "$<TARGET_FILE_DIR:MCPVET>"
    )
endif()
if(MINGW)
    set_target_properties(MCPVET PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_MINGW "${CMAKE_BINARY_DIR}/bin"
    )
endif()

# 链接库
target_link_libraries(MCPVET PRIVATE
    project_options
    project_warnings
    core
    io
    ui
    render
    glad
    dearimgui
    OpenGL::GL
)

target_compile_definitions(MCPVET PRIVATE IMGUI_DEFINE_MATH_OPERATORS)

# GLFW库链接（已注释）
# target_link_libraries(MCPVET PRIVATE glfw opengl32 gdi32 winmm)

# 包含目录
target_include_directories(MCPVET PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/imgui
    src/
)

# 单元测试
include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

# 复制必要的资源文件到输出目录
if(UNIX OR MSYS OR MINGW OR APPLE)
    add_custom_command(
        TARGET MCPVET
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:MCPVET>/init"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/init"
            "$<TARGET_FILE_DIR:MCPVET>/init"
        COMMENT "Copying init directory to output directory"
        VERBATIM
    )


endif()
