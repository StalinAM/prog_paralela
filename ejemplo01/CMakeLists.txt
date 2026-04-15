cmake_minimum_required(VERSION 3.16)

project(Ejemplo01)
find_package(fmt CONFIG REQUIRED)

add_executable(Ejemplo01 main.cpp)
target_link_libraries(Ejemplo01 PRIVATE fmt::fmt)