cmake_minimum_required(VERSION 3.16)
project(Lab3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(Lab3 MACOSX_BUNDLE
    Lab3.cpp
    canvas.h
    canvas.cpp
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
)

target_link_libraries(Lab3 PRIVATE Qt6::Widgets)