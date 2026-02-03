#ifndef SAVEPATH_H
#define SAVEPATH_H

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

// 获取当前可执行文件的完整路径（跨平台）
// Windows: 使用 GetModuleFileNameW
// macOS: 使用 _NSGetExecutablePath
// Linux/Unix: 使用 readlink /proc/self/exe
// 如果无法获取，抛出 std::runtime_error
fs::path getExecutablePath();

// 获取当前可执行文件所在的目录（跨平台）
// 调用 getExecutablePath() 并返回 parent_path()
// 如果无法获取，抛出 std::runtime_error
fs::path getExecutableDirectory();

// 获取当前可执行文件所在的目录，返回字符串引用（线程安全，一次性计算）
// 使用魔法静态变量确保只计算一次，线程安全
// 如果无法获取，返回空字符串（避免异常传播到未知上下文）
const std::string& getExecutableDirectoryString();

#endif // SAVEPATH_H