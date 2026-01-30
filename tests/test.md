build\tests\ 目录包含编译生成的 GoogleTest 可执行文件：
运行全部测试

# 从项目根目录运行

ctest --test-dir build --output-on-failure

# 或直接进入 build 目录

cd build && ctest --output-on-failure
按模块运行特定测试

# 只运行 core 模块测试

ctest --test-dir build -R test_core --output-on-failure

# 运行多个模块

ctest --test-dir build -R "test_core|test_io" --output-on-failure
直接运行单个测试二进制

# 从项目根目录

build\tests\test_core.exe

# 带详细输出

build\tests\test_core.exe --gtest_color=yes

# 列出所有测试用例

build\tests\test_core.exe --gtest_list_tests

# 运行特定测试套件

build\tests\test_core.exe --gtest_filter="GeometryFactoryTest.*"

# 运行单个测试

build\tests\test_core.exe --gtest_filter="LogManagerTest.LogEntry"
常用 gtest 选项

- --gtest_repeat=N：重复运行 N 次
- --gtest_shuffle：随机顺序运行测试
- --gtest_output=xml：输出 XML 报告
- --gtest_break_on_failure：失败时暂停
  测试架构说明
- test_core.exe：核心模块（几何、日志、坐标系）
- test_io.exe：输入输出模块（配置、场景管理）
- test_render.exe：渲染模块（Framebuffer、渲染管线）
- test_ui.exe：UI 模块（窗口、输入控制）
  重新构建测试
  如需重新启用测试，需配置 CMake：
  cmake -S . -B build -DBUILD_TESTING=ON
  cd build && mingw32-make
  测试结果将显示通过/失败数量和具体失败信息。使用 --output-on-failure 可在失败时输出详细信息。
