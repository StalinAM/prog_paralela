cmake_minimum_required(VERSION 3.16)

project(FractalJulia)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -fopenmp") 

find_package(fmt CONFIG REQUIRED)
find_package(SFML COMPONENTS Graphics Window Audio System CONFIG REQUIRED)

add_executable(fractal-julia main.cpp fractal_serial.cpp palette.cpp fractal_simd.cpp fractal_openmp.cpp)

target_link_libraries(fractal-julia PRIVATE fmt::fmt SFML::Graphics SFML::Window SFML::Audio SFML::System)