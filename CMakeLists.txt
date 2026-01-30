cmake_minimum_required(VERSION 3.15)

project(MCNPApp
    VERSION 0.1.0
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 统一依赖管理，在 add_subdirectory 之前
include(cmake/dependencies.cmake)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

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
add_executable(MCNPAppExec
    src/main.cpp
    ${CMAKE_SOURCE_DIR}/include/imgui/ImGuiFileDialog.cpp
)

# 统一输出目录，确保资源路径如models/ 正确
set_target_properties(MCNPAppExec PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/bin"
)
if(MSVC)
    set_target_properties(MCNPAppExec PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:MCNPAppExec>"
    )
endif()
if(XCODE)
    set_target_properties(MCNPAppExec PROPERTIES
        XCODE_SCHEME_WORKING_DIRECTORY "$<TARGET_FILE_DIR:MCNPAppExec>"
    )
endif()
if(MINGW)
    set_target_properties(MCNPAppExec PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_MINGW "${CMAKE_BINARY_DIR}/bin"
    )
endif()

# 链接库
target_link_libraries(MCNPAppExec PRIVATE
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

# GLFW库链接（已注释）
# target_link_libraries(MCNPAppExec PRIVATE glfw opengl32 gdi32 winmm)

# 包含目录
target_include_directories(MCNPAppExec PRIVATE
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
if(UNIX OR MSYS)
    add_custom_command(
        TARGET MCNPAppExec
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:MCNPAppExec>/init"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/init"
            "$<TARGET_FILE_DIR:MCNPAppExec>/init"
        COMMENT "Copying init directory to output directory"
        VERBATIM
    )


endif()
