# UI module library
add_library(ui
    ui_interface.cpp
    input_control.cpp
    transform_controller.cpp
    language_manager.cpp
    language_manager.h
    MWindows.h
    MWindows.cpp
    UILayoutManager.h
    UILayoutManager.cpp
    panels/TopBarWindow.h
    panels/SideBarWindow.h
    panels/BottomBarWindow.h
    panels/ViewportWindow.h
    ${CMAKE_SOURCE_DIR}/include/imgui/ImGuiFileDialog.cpp
)

# 导出接口包含目录
target_include_directories(ui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接依赖库
target_link_libraries(ui
    PUBLIC
        project_options
        project_warnings
        core
        io
        render
        dearimgui
)