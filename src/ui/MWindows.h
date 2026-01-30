#pragma once
#include <string>
#include <imgui.h>

namespace mcnp::ui {

struct WindowRect {
    ImVec2 pos{0, 0}; // 窗口位置
    ImVec2 size{300, 200}; // 窗口大小
};

class MWindows {
public:
    explicit MWindows(std::string title)
        : title_(std::move(title)) {}

    virtual ~MWindows() = default;

    // --- 基本属性 ---
    const std::string& Title() const noexcept { return title_; }
    void SetTitle(std::string t) { title_ = std::move(t); }

    void SetVisible(bool v) noexcept { visible_ = v; }
    bool IsVisible() const noexcept { return visible_; }
    bool* VisiblePtr() noexcept { return &visible_; }

    void SetRect(const WindowRect& r) noexcept { rect_ = r; }
    const WindowRect& GetRect() const noexcept { return rect_; }

    // --- ImGui flags / 行为 ---
    void SetFlags(ImGuiWindowFlags f) noexcept { flags_ = f; }
    ImGuiWindowFlags Flags() const noexcept { return flags_; }

    // 是否在渲染前应用 NextWindowPos/Size。
    // 注意：如果每帧都用 ImGuiCond_Always 强制应用，会导致窗口无法拖拽/缩放。
    // 这里把“条件”单独暴露出来，默认仅在 FirstUse 时应用，既保留默认布局，又允许用户调整。
    void SetForceApplyLayout(bool v) noexcept { forceApplyLayout_ = v; }
    void SetApplyLayoutCond(ImGuiCond cond) noexcept { applyCond_ = cond; }

    // 在渲染前设置 NextWindowPos/Size
    virtual void ApplyLayout()
    {
        if (!forceApplyLayout_) return;
        ImGui::SetNextWindowPos(rect_.pos, applyCond_);
        ImGui::SetNextWindowSize(rect_.size, applyCond_);
    }

    // 主入口：每帧调用
    void Draw()
    {
        if (!visible_) return;
        ApplyLayout();
        OnBegin();
        if (ImGui::Begin(title_.c_str(), closable_ ? &visible_ : nullptr, flags_)) {
            OnDraw();
        }
        // 将用户交互后的窗口位置/尺寸回写，便于持久化或后续“状态栏显示视图大小”等需求。
        rect_.pos = ImGui::GetWindowPos();
        rect_.size = ImGui::GetWindowSize();
        ImGui::End();
        OnEnd();
    }

    void SetClosable(bool c) noexcept { closable_ = c; }

protected:
    // Begin/End 钩子：例如统计、DockSpace、样式push/pop
    virtual void OnBegin() {}
    virtual void OnEnd() {}

    // 派生类实现 UI 内容
    virtual void OnDraw() = 0;

    std::string title_;
    WindowRect rect_;
    bool visible_{true};
    bool closable_{false};
    bool forceApplyLayout_{true};
    ImGuiCond applyCond_{ImGuiCond_FirstUseEver};
    ImGuiWindowFlags flags_{ImGuiWindowFlags_NoCollapse};
};

} // namespace mcnp::ui
