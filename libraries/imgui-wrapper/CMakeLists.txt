cmake_minimum_required(VERSION 3.0)

add_library(imgui STATIC
    imgui/imgui.cpp
	imgui/imgui_widgets.cpp
	imgui/imgui_tables.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_demo.cpp
    imgui/backends/imgui_impl_win32.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

set_property(TARGET imgui PROPERTY CXX_STANDARD 17)
target_include_directories(imgui PUBLIC imgui imgui/backends)