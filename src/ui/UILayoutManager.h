#pragma once

#include <imgui.h>
#ifndef IMGUI_DISABLE
#include <imgui_internal.h> // DockBuilder API
#endif
#include <iostream>

#include "panels/TopBarWindow.h"
#include "panels/SideBarWindow.h"
#include "panels/BottomBarWindow.h"
#include "panels/ViewportWindow.h"

namespace mcnp::ui {

// 基于 DockSpace 的布局管理器：
// - 解决"子类窗口无法拖拽改变大小"问题（不再每帧强制 SetNextWindowSize/Pos）。
// - 保持默认布局：左侧 Side、底部 Bottom、中间 Viewport。
// - Top 使用 MainMenuBar（仍复用 TopBarWindow 内的菜单逻辑）。
class UILayoutManager {
public:
    UILayoutManager(TopBarWindow& top, SideBarWindow& side, BottomBarWindow& bottom, ViewportWindow& viewport)
        : top_(top), side_(side), bottom_(bottom), viewport_(viewport)
    {
        // Dock 布局下不应强制每帧应用 Layout
        side_.SetForceApplyLayout(false);
        bottom_.SetForceApplyLayout(false);
        viewport_.SetForceApplyLayout(false);
        top_.SetForceApplyLayout(false);

        // 允许用户拖拽/缩放（由各窗口自身 flags 控制）
        side_.SetApplyLayoutCond(ImGuiCond_FirstUseEver);
        bottom_.SetApplyLayoutCond(ImGuiCond_FirstUseEver);
        viewport_.SetApplyLayoutCond(ImGuiCond_FirstUseEver);
    }

    void DrawAll()
    {
        static bool firstDraw = true;
        if (firstDraw) {
            firstDraw = false;
            // Log initial layout for debugging
            // LogManager::getInstance()->logOperation("Layout", "First draw of UI layout");
        }
        DrawMainMenuBar();
        DrawDockSpace();
        if (top_.ConsumeResetLayoutRequest()) {
            layoutBuilt_ = false;
        }
        side_.Draw();
        viewport_.Draw();
        bottom_.Draw();
    }

private:
    void DrawMainMenuBar()
    {
        // 复用 TopBarWindow 的菜单逻辑
        top_.DrawMainMenuBar();
    }

    void DrawDockSpace()
    {
        ImGuiIO& io = ImGui::GetIO();
        (void)io;

        const ImGuiViewport* vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(vp->WorkPos);
        ImGui::SetNextWindowSize(vp->WorkSize);
        
        #ifdef IMGUI_HAS_DOCK
        #define IMGUI_HAS_VIEWPORT
        #endif
        
        #ifdef IMGUI_HAS_VIEWPORT
        ImGui::SetNextWindowViewport(vp->ID);
        #endif

        ImGuiWindowFlags hostFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
                                    #ifdef IMGUI_HAS_DOCK
                                    ImGuiWindowFlags_NoDocking |
                                    #endif
                                     0; // ImGuiWindowFlags_NoSavedSettings removed

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("##DockSpaceHost", nullptr, hostFlags);
        ImGui::PopStyleVar(3);

        #ifdef IMGUI_HAS_DOCK
        const ImGuiID dockspaceId = ImGui::GetID("MCNPAppDockSpace");
        ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockFlags);

        if (!layoutBuilt_) {
            BuildDefaultLayout(dockspaceId);
            layoutBuilt_ = true;
        }
        #else
        // 如果没有Docking功能，使用手动布局
        // 侧边栏左侧20%，底部栏底部15%，视口占据剩余空间
        // vp->WorkPos/Size 已经考虑了菜单栏
        const ImVec2 workPos = vp->WorkPos;
        const ImVec2 workSize = vp->WorkSize;
        
        const float sideWidth = workSize.x * 0.2f;
        const float bottomHeight = workSize.y * 0.15f;
        const float viewportHeight = workSize.y - bottomHeight;
        
        static bool firstLayout = true;
        if (firstLayout) {
            firstLayout = false;
            std::cout << "[Layout] Manual layout calculation:" << std::endl;
            std::cout << "  WorkPos: " << workPos.x << "," << workPos.y << std::endl;
            std::cout << "  WorkSize: " << workSize.x << "x" << workSize.y << std::endl;
            std::cout << "  SideWidth: " << sideWidth << std::endl;
            std::cout << "  BottomHeight: " << bottomHeight << std::endl;
            std::cout << "  ViewportHeight: " << viewportHeight << std::endl;
        }
        
        // 按照 DrawAll() 中的绘制顺序设置窗口位置和大小
        // 1. 侧边栏 - 左侧，高度为viewportHeight
        ImGui::SetNextWindowPos(ImVec2(workPos.x, workPos.y), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(sideWidth, viewportHeight), ImGuiCond_FirstUseEver);
        
        // 2. 视口窗口 - 右侧剩余空间
        ImGui::SetNextWindowPos(ImVec2(workPos.x + sideWidth, workPos.y), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(workSize.x - sideWidth, viewportHeight), ImGuiCond_FirstUseEver);
        
        // 3. 底部栏 - 底部，宽度为剩余宽度
        ImGui::SetNextWindowPos(ImVec2(workPos.x + sideWidth, workPos.y + viewportHeight), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(workSize.x - sideWidth, bottomHeight), ImGuiCond_FirstUseEver);
        
        // 注意：窗口的实际绘制在 DrawAll() 中进行，这里只设置位置和大小
        #endif

        ImGui::End();
    }

    void BuildDefaultLayout(ImGuiID dockspaceId)
    {
        #ifdef IMGUI_HAS_DOCK
        // 初次运行建立默认 Dock 布局
        ImGui::DockBuilderRemoveNode(dockspaceId);
        ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->WorkSize);

        ImGuiID dockMain = dockspaceId;
        ImGuiID dockLeft = 0;
        ImGuiID dockBottom = 0;

        dockLeft = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.25f, nullptr, &dockMain);
        dockBottom = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Down, 0.25f, nullptr, &dockMain);

        ImGui::DockBuilderDockWindow(side_.Title().c_str(), dockLeft);
        ImGui::DockBuilderDockWindow(bottom_.Title().c_str(), dockBottom);
        ImGui::DockBuilderDockWindow(viewport_.Title().c_str(), dockMain);

        ImGui::DockBuilderFinish(dockspaceId);
        #endif
    }

private:
    TopBarWindow& top_;
    SideBarWindow& side_;
    BottomBarWindow& bottom_;
    ViewportWindow& viewport_;
    bool layoutBuilt_{false};
};

} // namespace mcnp::ui