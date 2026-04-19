cmake_minimum_required(VERSION 3.16)

project(ejemplo02)

find_package(fmt CONFIG REQUIRED)
find_package(SFML COMPONENTS Network Graphics Window Audio System CONFIG REQUIRED)

add_executable(ejemplo02 main.cpp)

target_link_libraries(ejemplo02 PRIVATE SFML::Network SFML::Graphics SFML::Window SFML::Audio SFML::System)