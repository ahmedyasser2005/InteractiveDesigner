cmake_minimum_required(VERSION 3.20)
project(InteractiveDesigner VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find DirectX 11 (Included in Windows SDK)
# Standard libraries needed for DX11 and Win32
set(DX_LIBS d3d11.lib dxgi.lib d3dcompiler.lib)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

# Create the executable
# WIN32 flag ensures it starts in WinMain, not main()
add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE src)

# Link Windows libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${DX_LIBS})

# Set the startup project for Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})