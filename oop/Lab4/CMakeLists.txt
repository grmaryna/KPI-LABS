cmake_minimum_required(VERSION 3.16)
project(Lab4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(Lab4 MACOSX_BUNDLE
    Lab4.cpp
    canvas.h
    canvas.cpp
    my_editor.h
    my_editor.cpp
    shape_registry.h
    shape_registry.cpp
    shape_parts.h
    shape.h
    shape.cpp
    point_shape.h
    point_shape.cpp
    line_shape.h
    line_shape.cpp
    rect_shape.h
    rect_shape.cpp
    ellipse_shape.h
    ellipse_shape.cpp
    line_oo_shape.h
    line_oo_shape.cpp
    cube_shape.h
    cube_shape.cpp
)

target_link_libraries(Lab4 PRIVATE Qt6::Widgets)
