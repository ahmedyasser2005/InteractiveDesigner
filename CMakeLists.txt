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

add_subdirectory(thirdparty)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "src")
target_link_libraries(${PROJECT_NAME} PRIVATE imgui Shcore.lib)