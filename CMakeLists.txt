cmake_minimum_required(VERSION 3.25)

project("InteractiveDesigner" 
	VERSION 1.0 
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
	add_compile_options(/Zc:preprocessor /W4 /utf-8 /permissive-)
    add_compile_definitions(UNICODE _UNICODE)
endif()

set(SOURCES
	"src/main.cpp"

    "src/Platform/Window.cpp"
    "src/Platform/Window.h"
	
	"src/Renderer/DX11Renderer.cpp"
	"src/Renderer/DX11Renderer.h"
    "src/Renderer/Utility.h"

    "src/Engine/Engine.cpp"
    "src/Engine/Engine.h"
)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE 
	d3d11.lib 
	dxgi.lib 
	d3dcompiler.lib
)


# Compile & link HLSL shaders
find_program(FXC_EXECUTABLE fxc PATHS "$ENV{WindowsSdkDir}bin/$ENV{WindowsSDKVersion}x64")

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/VertexShader.cso"
    COMMAND ${FXC_EXECUTABLE} /T vs_5_0 /E VS /Fo "${CMAKE_CURRENT_BINARY_DIR}/VertexShader.cso" "${CMAKE_CURRENT_SOURCE_DIR}/src/Shaders/VertexShader.hlsl"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/Shaders/VertexShader.hlsl"
)
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/PixelShader.cso"
    COMMAND ${FXC_EXECUTABLE} /T ps_5_0 /E PS /Fo "${CMAKE_CURRENT_BINARY_DIR}/PixelShader.cso" "${CMAKE_CURRENT_SOURCE_DIR}/src/Shaders/PixelShader.hlsl"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/Shaders/PixelShader.hlsl"
)
target_sources(${PROJECT_NAME} PRIVATE 
    "${CMAKE_CURRENT_BINARY_DIR}/VertexShader.cso"
    "${CMAKE_CURRENT_BINARY_DIR}/PixelShader.cso"
)
