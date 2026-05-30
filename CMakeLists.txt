cmake_minimum_required(VERSION 3.25)
project(InteractiveDesigner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(
        /Zc:preprocessor
        /W4
        /WX
        /utf-8
        /permissive-
    )
    add_compile_definitions(UNICODE _UNICODE)
endif()

add_subdirectory(thirdparty)

add_library(app_core STATIC)

target_sources(app_core
    PRIVATE
        "src/Platform/Graphics.cpp"
        "src/Platform/Window.cpp"
        "src/EditorUI/GUI.cpp"
        "src/Application/Application.cpp"
)

target_include_directories(app_core
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(app_core 
    PUBLIC 
        imgui
    PRIVATE
        Shcore.lib
)

add_executable(${PROJECT_NAME} WIN32)

target_sources(${PROJECT_NAME}
    PRIVATE
        "src/WinMain.cpp"
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        app_core
)

option(BUILD_TESTING "Build the unit test suite" ON)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()