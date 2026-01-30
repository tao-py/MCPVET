#pragma once
#include "../MWindows.h"
#include "../../render/Framebuffer.h"

namespace mcnp::ui {

// // 渲染回调签名，修改时注意同步声明
using RenderCallback = void(*)(int viewportW, int viewportH);

class ViewportWindow final : public MWindows {
public:
    explicit ViewportWindow(RenderCallback cb) // cb: 渲染回调, viewport 纹理
        : MWindows("Viewport")
        , render_(cb)
    {
        SetClosable(false);
        SetFlags(ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    }

    // 是否被鼠标悬停
    bool IsHovered() const noexcept { return hovered_; }
    ImVec2 ViewportSize() const noexcept { return ImVec2((float)lastWidth_, (float)lastHeight_); }
    bool GetImageRect(ImVec2& outPos, ImVec2& outSize) const noexcept
    {
        if (imageSize_.x <= 0.0f || imageSize_.y <= 0.0f) return false;
        outPos = imagePos_;
        outSize = imageSize_;
        return true;
    }

private:
    void OnDraw() override
    {
        // // 获取可用区域尺寸
        const ImVec2 avail = ImGui::GetContentRegionAvail();
        // 设置最小尺寸，避免视口过小
        const int minWidth = 400;
        const int minHeight = 300;
        const int w = std::max(minWidth, (int)avail.x);
        const int h = std::max(minHeight, (int)avail.y);

        // resize FBO
        if (w > 0 && h > 0) {
            fbo_.Resize(w, h);

            // 保存当前 OpenGL viewport（否则 ImGui 渲染可能被 FBO viewport 影响）
            GLint prevViewport[4] = {0, 0, 0, 0};
            glGetIntegerv(GL_VIEWPORT, prevViewport);

            // 1) // 离屏渲染到 FBO
            fbo_.Bind();
            glViewport(0, 0, fbo_.Width(), fbo_.Height());
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (render_) {
                render_(fbo_.Width(), fbo_.Height());
            }

            mcnp::render::Framebuffer::Unbind();

            // 恢复 viewport
            glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);

            // 显示 FBO 的颜色纹理
            // // 注意 OpenGL 纹理坐标 Y 需要翻转
            ImGui::Image((ImTextureID)(intptr_t)fbo_.ColorTexture(), avail, ImVec2(0,1), ImVec2(1,0));
            imagePos_ = ImGui::GetItemRectMin();
            ImVec2 maxPos = ImGui::GetItemRectMax();
            imageSize_ = ImVec2(maxPos.x - imagePos_.x, maxPos.y - imagePos_.y);
        } else {
            ImGui::TextUnformatted("Viewport 尺寸无效");
            imageSize_ = ImVec2(0.0f, 0.0f);
        }

        hovered_ = ImGui::IsItemHovered();
    }

    mcnp::render::Framebuffer fbo_;
    RenderCallback render_{nullptr};
    bool hovered_{false};
    int lastWidth_{0};
    int lastHeight_{0};
    ImVec2 imagePos_{0.0f, 0.0f};
    ImVec2 imageSize_{0.0f, 0.0f};
};

} // namespace mcnp::ui

