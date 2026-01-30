#pragma once
#include <glad/glad.h>
#include <cstdint>
#include <cstdio>

namespace mcnp::render {

class Framebuffer {
public:
    Framebuffer() = default;
    ~Framebuffer() { Destroy(); }

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    void Create(int w, int h)
    {
        Destroy();
        width_ = w; height_ = h;

        glGenFramebuffers(1, &fbo_);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

        glGenTextures(1, &colorTex_);
        glBindTexture(GL_TEXTURE_2D, colorTex_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex_, 0);

        glGenRenderbuffers(1, &rboDepth_);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepth_);

        // 完整性检查：如果失败，后续 viewport 可能显示黑屏
        const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            // 这里不能直接依赖 LogManager（render 模块保持低耦合），用 stderr。
            // 上层可在 DEBUG 模式通过 GL debug callback 进一步定位。
            std::fprintf(stderr, "[Framebuffer] Incomplete FBO status=0x%X (w=%d h=%d)\n", status, width_, height_);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Resize(int w, int h)
    {
        if (w <= 0 || h <= 0) return;
        if (w == width_ && h == height_) return;
        Create(w, h);
    }

    void Bind() const { glBindFramebuffer(GL_FRAMEBUFFER, fbo_); }
    static void Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

    GLuint ColorTexture() const noexcept { return colorTex_; }
    int Width() const noexcept { return width_; }
    int Height() const noexcept { return height_; }

private:
    void Destroy()
    {
        if (rboDepth_) { glDeleteRenderbuffers(1, &rboDepth_); rboDepth_ = 0; }
        if (colorTex_) { glDeleteTextures(1, &colorTex_); colorTex_ = 0; }
        if (fbo_) { glDeleteFramebuffers(1, &fbo_); fbo_ = 0; }
        width_ = height_ = 0;
    }

    GLuint fbo_{0};
    GLuint colorTex_{0};
    GLuint rboDepth_{0};
    int width_{0};
    int height_{0};
};

} // namespace mcnp::render