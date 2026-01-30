# 启用测试
enable_testing()

# 查找Google Test（如果系统中有安装）
find_package(GTest QUIET)

if(GTest_FOUND)
    message(STATUS "Using system Google Test")
    set(TEST_LIBRARIES GTest::gtest GTest::gtest_main)
else()
    message(WARNING "System Google Test not found, using bundled version or FetchContent")
    
    # 使用FetchContent获取Google Test
    include(FetchContent)
    
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    
    FetchContent_MakeAvailable(googletest)
    
    set(TEST_LIBRARIES GTest::gtest GTest::gtest_main)
endif()

# 测试core模块
add_executable(test_core
    test_core.cpp
)

target_link_libraries(test_core PRIVATE core ${TEST_LIBRARIES})
add_test(NAME test_core COMMAND test_core)

# 测试io模块
add_executable(test_io
    test_io.cpp
)

target_link_libraries(test_io PRIVATE io ${TEST_LIBRARIES})
add_test(NAME test_io COMMAND test_io)

# 测试ui模块
add_executable(test_ui
    test_ui.cpp
)

target_link_libraries(test_ui PRIVATE ui ${TEST_LIBRARIES})
add_test(NAME test_ui COMMAND test_ui)

# 测试render模块
add_executable(test_render
    test_render.cpp
)

target_link_libraries(test_render PRIVATE render ${TEST_LIBRARIES})
add_test(NAME test_render COMMAND test_render)