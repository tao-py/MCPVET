#include "savepath.h"
#include <iostream>
#include <vector>
#include <stdexcept>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <limits.h>
#endif

#if defined(__APPLE__) && defined(__MACH__)
    #include <mach-o/dyld.h>
#endif

namespace fs = std::filesystem;

fs::path getExecutablePath() {
    fs::path exePath;
#ifdef _WIN32
    // Windows: 使用 GetModuleFileNameW 支持 Unicode 路径
    std::vector<wchar_t> buffer;
    DWORD size = MAX_PATH;
    while (true) {
        buffer.resize(size);
        DWORD result = GetModuleFileNameW(nullptr, buffer.data(), size);
        if (result == 0) {
            throw std::runtime_error("无法获取可执行文件路径");
        }
        if (result < size) {
            buffer.resize(result);
            break;
        }
        size *= 2;  // 缓冲区不够，扩大后再试
    }
    exePath = fs::path(std::wstring(buffer.begin(), buffer.end()));
#elif defined(__APPLE__) && defined(__MACH__)
    // macOS: 使用 _NSGetExecutablePath
    uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);  // 获取所需大小
    std::vector<char> buffer(size);
    if (_NSGetExecutablePath(buffer.data(), &size) != 0) {
        throw std::runtime_error("无法获取可执行文件路径");
    }
    exePath = fs::path(buffer.begin(), buffer.end());
#else
    // Linux 和其他类 Unix 系统: 使用 readlink /proc/self/exe
    std::vector<char> buffer(PATH_MAX);
    ssize_t len = readlink("/proc/self/exe", buffer.data(), buffer.size());
    if (len == -1) {
        throw std::runtime_error("无法获取可执行文件路径");
    }
    buffer.resize(len);
    exePath = fs::path(buffer.begin(), buffer.end());
#endif

    // 尝试解析符号链接（非 Windows 平台）
#ifndef _WIN32
    try {
        if (fs::exists(exePath) && fs::is_symlink(exePath)) {
            exePath = fs::canonical(exePath);
        }
    } catch (const fs::filesystem_error&) {
        // 忽略解析失败，返回原始路径
    }
#endif
    return exePath;
}

fs::path getExecutableDirectory() {
    return getExecutablePath().parent_path();
}

const std::string& getExecutableDirectoryString() {
    static const std::string dir = []() -> std::string {
        try {
            return getExecutableDirectory().string();
        } catch (const std::exception&) {
            // 如果无法获取，返回空字符串，避免异常传播
            return "";
        }
    }();
    return dir;
}

// 保留测试主函数（可选）
#ifdef SAVEPATH_TEST
int main() {
    try {
        fs::path exePath = getExecutablePath();
        fs::path exeDir = getExecutableDirectory();
        std::string exeDirStr = getExecutableDirectoryString();
        
        std::cout << "完整路径: " << exePath << std::endl;
        std::cout << "所在目录: " << exeDir << std::endl;
        std::cout << "目录字符串: " << exeDirStr << std::endl;
        std::cout << "文件名: " << exePath.filename() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
#endif