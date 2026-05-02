add_library(imgui STATIC
    "imgui/imgui.cpp"
    "imgui/imgui_demo.cpp"
    "imgui/imgui_draw.cpp"
    "imgui/imgui_tables.cpp"
    "imgui/imgui_widgets.cpp"
    "imgui/backends/imgui_impl_dx11.cpp"
    "imgui/backends/imgui_impl_win32.cpp"
)

target_include_directories(imgui PUBLIC 
    "imgui" 
    "imgui/backends"
)

target_link_libraries(imgui PUBLIC
    d3d11
    dxgi
    d3dcompiler
)