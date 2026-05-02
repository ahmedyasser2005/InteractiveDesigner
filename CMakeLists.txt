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
	"src/Platform/CustomWin.h"
	"src/Platform/Input.h"
	"src/Platform/Window.h"
	"src/Platform/Window.cpp"
	"src/Application/Application.h"
	"src/Application/Application.cpp"
	"src/Graphics/Graphics.h"
	"src/Graphics/Graphics.cpp"
	"src/Utils/ErrorUtils.h"
)

add_executable(${PROJECT_NAME} # WIN32
	${SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE 
	d3d11.lib 
	dxgi.lib 
	d3dcompiler.lib
	Shcore.lib
)