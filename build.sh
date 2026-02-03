#!/bin/bash

# MCPVET Cross-Platform Build and Run Script
echo "========================================"
echo "      MCPVET Build and Run Script"
echo "========================================"

# Detect operating system and architecture
UNAME_S=$(uname -s)
UNAME_M=$(uname -m)

echo "Platform: $UNAME_S $UNAME_M"

# Determine CMake generator, build tool, and executable extension
CMAKE_GEN=""
BUILD_TOOL=""
EXE_EXT=""

case "$UNAME_S" in
    Linux*)
        CMAKE_GEN="Unix Makefiles"
        BUILD_TOOL="make"
        EXE_EXT=""
        ;;
    Darwin*)
        CMAKE_GEN="Unix Makefiles"
        BUILD_TOOL="make"
        EXE_EXT=""
        ;;
    MINGW*|MSYS*|CYGWIN*)
        # Windows with MinGW/MSYS/Cygwin
        CMAKE_GEN="MinGW Makefiles"
        BUILD_TOOL="mingw32-make"
        EXE_EXT=".exe"
        ;;
    *)
        # Fallback for unknown systems
        echo "Warning: Unknown OS '$UNAME_S', using default Unix Makefiles"
        CMAKE_GEN="Unix Makefiles"
        BUILD_TOOL="make"
        EXE_EXT=""
        ;;
esac

# Allow environment variable override
if [ -n "$CMAKE_GENERATOR" ]; then
    CMAKE_GEN="$CMAKE_GENERATOR"
    echo "Using CMake generator from environment: $CMAKE_GEN"
fi

if [ -n "$CMAKE_BUILD_TOOL" ]; then
    BUILD_TOOL="$CMAKE_BUILD_TOOL"
    echo "Using build tool from environment: $BUILD_TOOL"
fi

# Check if running in project root directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "Error: Please run this script in the project root directory."
    read -p "Press Enter to exit..."
    exit 1
fi

# Check if build directory exists
if [ ! -d "build/" ]; then
    echo "Build directory does not exist, running CMake configuration..."
    cmake -S . -B build -G "$CMAKE_GEN"
    if [ $? -ne 0 ]; then
        echo "CMake configuration failed."
        read -p "Press Enter to exit..."
        exit 1
    fi
    echo "CMake configuration succeeded."
else
    echo "Build directory detected."
    # Check if build files exist (Makefile for Unix Makefiles, build.ninja for Ninja, etc.)
    if [ ! -f "build/Makefile" ] && [ ! -f "build/build.ninja" ]; then
        echo "Build files missing, re-running CMake configuration..."
        cmake -S . -B build -G "$CMAKE_GEN"
        if [ $? -ne 0 ]; then
            echo "CMake configuration failed."
            read -p "Press Enter to exit..."
            exit 1
        fi
        echo "CMake configuration succeeded."
    fi
fi

# Ask user if they want to build
echo
read -p "Execute $BUILD_TOOL for build? (y/n): " make_choice
if [[ ! "$make_choice" =~ ^[Yy]$ ]]; then
    echo "Skipping build."
else
    echo "Building..."
    cd build
    $BUILD_TOOL
    if [ $? -ne 0 ]; then
        echo "Build failed."
        cd ..
        read -p "Press Enter to exit..."
        exit 1
    fi
    cd ..
    echo "Build succeeded."
fi

# Ask user if they want to run the program
echo
read -p "Run program MCPVET? (y/n): " run_choice
if [[ ! "$run_choice" =~ ^[Yy]$ ]]; then
    echo "Skipping execution."
    read -p "Press Enter to exit..."
    exit 0
fi

# Check if executable exists
EXECUTABLE="build/bin/MCPVET$EXE_EXT"
if [ ! -f "$EXECUTABLE" ]; then
    echo "Error: Executable '$EXECUTABLE' not found. Please build first."
    read -p "Press Enter to exit..."
    exit 1
fi

echo "Running program..."
case "$UNAME_S" in
    Darwin*)
        open -a "$PWD/$EXECUTABLE"
        ;;
    MINGW*|MSYS*|CYGWIN*)
        cd build/bin
        start "" "MCPVET.exe"
        cd ../..
        ;;
    *)
        # Linux and other Unix-like systems
        cd build/bin
        ./MCPVET &
        cd ../..
        ;;
esac

echo "Program started."
read -p "Press Enter to exit..."
exit 0