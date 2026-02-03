#pragma once

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>
#ifndef IMGUI_DISABLE
#include <imgui_internal.h> // DockBuilder API
#endif
#include <iostream>
#include <algorithm>

#include "panels/TopBarWindow.h"
#include "panels/SideBarWindow.h"
#include "panels/BottomBarWindow.h"
#include "panels/ViewportWindow.h"
#include <memory>

namespace mcnp::ui {

// 基于 DockSpace 的布局管理器：
// - 解决"子类窗口无法拖拽改变大小"问题（不再每帧强制 SetNextWindowSize/Pos）。
// - 保持默认布局：左侧 Side、底部 Bottom、中间 Viewport。
// - Top 使用 MainMenuBar（仍复用 TopBarWindow 内的菜单逻辑）。
class UILayoutManager {
public:
    // 重载构造函数，接收UI组件的unique_ptr并转移所有权
    UILayoutManager(std::unique_ptr<TopBarWindow> top, 
                   std::unique_ptr<SideBarWindow> side, 
                   std::unique_ptr<BottomBarWindow> bottom, 
                   std::unique_ptr<ViewportWindow> viewport)
        : topPtr_(std::move(top)), sidePtr_(std::move(side)), bottomPtr_(std::move(bottom)), viewportPtr_(std::move(viewport)),
          top_(*topPtr_), side_(*sidePtr_), bottom_(*bottomPtr_), viewport_(*viewportPtr_)
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

        #ifndef IMGUI_HAS_DOCK
        // 非 Docking 模式下固定布局，由分割条控制尺寸
        const ImGuiWindowFlags pinnedFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
        side_.SetFlags(side_.Flags() | pinnedFlags);
        bottom_.SetFlags(bottom_.Flags() | pinnedFlags);
        viewport_.SetFlags(viewport_.Flags() | pinnedFlags);
        side_.SetForceApplyLayout(true);
        bottom_.SetForceApplyLayout(true);
        viewport_.SetForceApplyLayout(true);
        side_.SetApplyLayoutCond(ImGuiCond_Always);
        bottom_.SetApplyLayoutCond(ImGuiCond_Always);
        viewport_.SetApplyLayoutCond(ImGuiCond_Always);
        #endif
    }

    // 保持原来的构造函数以兼容旧代码
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

        #ifndef IMGUI_HAS_DOCK
        // 非 Docking 模式下固定布局，由分割条控制尺寸
        const ImGuiWindowFlags pinnedFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
        side_.SetFlags(side_.Flags() | pinnedFlags);
        bottom_.SetFlags(bottom_.Flags() | pinnedFlags);
        viewport_.SetFlags(viewport_.Flags() | pinnedFlags);
        side_.SetForceApplyLayout(true);
        bottom_.SetForceApplyLayout(true);
        viewport_.SetForceApplyLayout(true);
        side_.SetApplyLayoutCond(ImGuiCond_Always);
        bottom_.SetApplyLayoutCond(ImGuiCond_Always);
        viewport_.SetApplyLayoutCond(ImGuiCond_Always);
        #endif
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
            manualLayout_.initialized = false;
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
        // VSCode 风格：左侧 Side 全高，右侧区域由 Viewport(上) + Bottom(下) 组成
        // vp->WorkPos/Size 已经考虑了菜单栏
        const ImVec2 workPos = vp->WorkPos;
        const ImVec2 workSize = vp->WorkSize;

        EnsureManualLayout(workSize);
        DrawManualSplitters(workPos, workSize);
        ApplyManualLayout(workPos, workSize);

        // 注意：窗口的实际绘制在 DrawAll() 中进行，这里只设置位置和大小
        #endif

        ImGui::End();
    }

    void BuildDefaultLayout([[maybe_unused]] ImGuiID dockspaceId)
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
    void EnsureManualLayout(const ImVec2& workSize)
    {
        const float maxSide = std::max(0.0f, workSize.x - kMinViewportWidth);
        const float maxBottom = std::max(0.0f, workSize.y - kMinViewportHeight);
        const float minSide = std::min(kMinSideWidth, maxSide);
        const float minBottom = std::min(kMinBottomHeight, maxBottom);

        if (!manualLayout_.initialized) {
            manualLayout_.sideWidth = std::clamp(side_.Width(), minSide, maxSide);
            manualLayout_.bottomHeight = std::clamp(bottom_.Height(), minBottom, maxBottom);
            manualLayout_.initialized = true;
        }

        manualLayout_.sideWidth = std::clamp(manualLayout_.sideWidth, minSide, maxSide);
        manualLayout_.bottomHeight = std::clamp(manualLayout_.bottomHeight, minBottom, maxBottom);
    }

    void DrawManualSplitters(const ImVec2& workPos, const ImVec2& workSize)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        const float splitterSize = 4.0f;

        // 垂直分割条：Side <-> 右侧区域
        const ImVec2 vSplitPos(workPos.x + manualLayout_.sideWidth - splitterSize * 0.5f, workPos.y);
        const ImVec2 vSplitSize(splitterSize, workSize.y);
        ImGui::SetCursorScreenPos(vSplitPos);
        ImGui::InvisibleButton("##SplitterVertical", vSplitSize);
        const bool vHovered = ImGui::IsItemHovered();
        const bool vActive = ImGui::IsItemActive();
        if (vActive) {
            manualLayout_.sideWidth += io.MouseDelta.x;
        }
        if (vHovered || vActive) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }

        // 水平分割条：Viewport <-> Bottom（仅右侧区域）
        const float rightWidth = workSize.x - manualLayout_.sideWidth;
        const ImVec2 hSplitPos(workPos.x + manualLayout_.sideWidth, workPos.y + (workSize.y - manualLayout_.bottomHeight) - splitterSize * 0.5f);
        const ImVec2 hSplitSize(rightWidth, splitterSize);
        ImGui::SetCursorScreenPos(hSplitPos);
        ImGui::InvisibleButton("##SplitterHorizontal", hSplitSize);
        const bool hHovered = ImGui::IsItemHovered();
        const bool hActive = ImGui::IsItemActive();
        if (hActive) {
            manualLayout_.bottomHeight -= io.MouseDelta.y;
        }
        if (hHovered || hActive) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
        }

        // 夹紧，保证 Viewport 最小尺寸
        EnsureManualLayout(workSize);

        const ImU32 vColor = ImGui::GetColorU32(vActive ? ImGuiCol_SeparatorActive : (vHovered ? ImGuiCol_SeparatorHovered : ImGuiCol_Separator));
        const ImU32 hColor = ImGui::GetColorU32(hActive ? ImGuiCol_SeparatorActive : (hHovered ? ImGuiCol_SeparatorHovered : ImGuiCol_Separator));
        drawList->AddRectFilled(vSplitPos, ImVec2(vSplitPos.x + splitterSize, vSplitPos.y + workSize.y), vColor);
        drawList->AddRectFilled(hSplitPos, ImVec2(hSplitPos.x + rightWidth, hSplitPos.y + splitterSize), hColor);
    }

    void ApplyManualLayout(const ImVec2& workPos, const ImVec2& workSize) const
    {
        const float sideWidth = manualLayout_.sideWidth;
        const float bottomHeight = manualLayout_.bottomHeight;
        const float viewportHeight = workSize.y - bottomHeight;

        // 按照 DrawAll() 中的绘制顺序设置窗口位置和大小
        // 1. 侧边栏 - 左侧，占满高度
        WindowRect sideRect;
        sideRect.pos = ImVec2(workPos.x, workPos.y);
        sideRect.size = ImVec2(sideWidth, workSize.y);
        side_.SetRect(sideRect);

        // 2. 视口窗口 - 右侧上方
        WindowRect viewportRect;
        viewportRect.pos = ImVec2(workPos.x + sideWidth, workPos.y);
        viewportRect.size = ImVec2(workSize.x - sideWidth, viewportHeight);
        viewport_.SetRect(viewportRect);

        // 3. 底部栏 - 右侧下方
        WindowRect bottomRect;
        bottomRect.pos = ImVec2(workPos.x + sideWidth, workPos.y + viewportHeight);
        bottomRect.size = ImVec2(workSize.x - sideWidth, bottomHeight);
        bottom_.SetRect(bottomRect);
    }

private:
    // 持有UI组件的所有权
    std::unique_ptr<TopBarWindow> topPtr_{nullptr};
    std::unique_ptr<SideBarWindow> sidePtr_{nullptr};
    std::unique_ptr<BottomBarWindow> bottomPtr_{nullptr};
    std::unique_ptr<ViewportWindow> viewportPtr_{nullptr};
    
    // 保持对UI组件的引用以兼容现有代码
    TopBarWindow& top_;
    SideBarWindow& side_;
    BottomBarWindow& bottom_;
    ViewportWindow& viewport_;
    bool layoutBuilt_{false};
    static constexpr float kMinSideWidth = 220.0f;
    static constexpr float kMinBottomHeight = 160.0f;
    static constexpr float kMinViewportWidth = 400.0f;
    static constexpr float kMinViewportHeight = 300.0f;
    struct ManualLayoutState {
        float sideWidth{320.0f};
        float bottomHeight{200.0f};
        bool initialized{false};
    };
    ManualLayoutState manualLayout_{};
};

} // namespace mcnp::ui
