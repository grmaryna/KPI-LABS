cmake_minimum_required(VERSION 3.16)
project(Lab5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Усі *.cpp/*.h беруться автоматично: щоб додати нову фігуру, достатньо
# покласти в папку її .h/.cpp — CMakeLists.txt змінювати не потрібно.
file(GLOB LAB5_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
                                          ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(Lab5 MACOSX_BUNDLE ${LAB5_SOURCES})
target_link_libraries(Lab5 PRIVATE Qt6::Widgets)
