cmake_minimum_required(VERSION 3.16)
project(Lab2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(Lab2
    Lab2.cpp
    shape.cpp
    point_shape.cpp
    line_shape.cpp
    rect_shape.cpp
    ellipse_shape.cpp
    canvas.cpp
)
target_link_libraries(Lab2 PRIVATE Qt6::Widgets)