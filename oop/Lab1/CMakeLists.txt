cmake_minimum_required(VERSION 3.16)
project(Lab1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(Lab1
    Lab1.cpp
    module1.cpp
    module1b.cpp
    module2.cpp
)
target_link_libraries(Lab1 PRIVATE Qt6::Widgets)