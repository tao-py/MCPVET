# MCPVET - 蒙卡程序可视化编辑工具

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![C++](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey)](platform)

**蒙特卡罗程序可视化编辑工具 (Monte Carlo Program Visualization Editor Tool)**

版本：MCPVET 1.0.0

## 📋 项目简介

MCNPApp 是一款专业的蒙特卡罗程序可视化编辑工具，主要用于 MCNP、FLUKA 和 Geant4 等蒙特卡罗模拟软件的可视化编辑。该工具提供直观的三维界面，使复杂的粒子物理模拟变得简单易懂。

## ✨ 主要功能

### 三维可视化编辑
- **基本几何体**：支持点、线、面、球体、长方体、圆柱体、圆锥等基本几何体的定义
- **几何体属性**：可设置源、受照射几何体和探测器等不同属性
- **布尔运算**：支持交、并、补等布尔运算操作
- **CAD风格操作**：采用类似 CAD 的指令快捷作图方式
- **多种源类型**：支持光源（X射线、伽马射线、光子）和粒子源（中子、α、β、辐射源）
- **材料定义**：支持有机材料、无机材料、混合物、金属等多种材料
- **探测器**：可记录光子、粒子的能量、通量、分布情况
- **交互操作**：支持几何体的复制、移动、拖拽、缩放、旋转、组合和锁定

### 输入卡导入导出
- **多格式支持**：支持 MCNP、FLUKA、Geant4 三种格式的输入文件导入导出
- **可视化转换**：可将文本格式的输入文件转换为可视化几何体

### 调试与运行
- **规则检查**：自动检查定义内容是否符合 MCNP 规则
- **路径检测**：自动搜索系统环境变量中的 MCNP 程序路径
- **错误报告**：提供详细的错误报告和调试信息

### 数据可视化
- **输出分析**：导入输出文件并进行可视化分析
- **结果展示**：直观展示模拟结果和数据分布

## 🛠 技术栈

- **编程语言**: C++17/C++20
- **图形界面**: [Dear ImGui](https://github.com/ocornut/imgui)
- **图形渲染**: OpenGL
- **数学库**: [GLM](https://github.com/g-truc/glm)
- **JSON解析**: [nlohmann/json](https://github.com/nlohmann/json)
- **构建系统**: CMake

## 🚀 快速开始

### 环境要求
- C++17 兼容的编译器 (GCC 7+, Clang 6+, MSVC 2017+)
- CMake 3.10+
- OpenGL 3.3+

### 构建步骤

1. 克隆项目
   ```bash
   git clone https://github.com/tao-py/MCPVET.git
   ```

2. 创建构建目录
   ```bash
   cd MCPVET
   mkdir build
   cd build
   ```

3. 配置项目
   ```bash
   cmake ..
   ```

4. 编译项目
   ```bash
   cmake --build .
   ```

## 🤝 贡献

欢迎提交 Pull Request 来改进此项目。对于重大更改，请先开 Issue 讨论您想要改变的内容。

## 📄 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件

## 📞 联系方式

如有任何问题或建议，请通过 GitHub Issues 联系我们。

## 🙏 致谢

- 感谢所有蒙特卡罗模拟领域的研究者和开发者
- 感谢所使用开源库的贡献者们

---

*蒙特卡罗程序可视化编辑工具 - 为科学计算可视化而生*