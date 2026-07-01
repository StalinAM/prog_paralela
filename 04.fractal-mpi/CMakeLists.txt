cmake_minimum_required(VERSION 3.16)

project(fractal-julia)

find_package(fmt CONFIG REQUIRED)
find_package(SFML COMPONENTS Graphics Window Audio System CONFIG REQUIRED)

set(MPI_ROOT "C:/Program Files (x86)/Intel/oneAPI/mpi/latest")
set(MPI_INCLUDE "${MPI_ROOT}/include")
set(MPI_LIB "${MPI_ROOT}/lib")

include_directories(
    ${MPI_INCLUDE}
    )
add_executable(fractal-julia main.cpp fractal_mpi.cpp palette.cpp)

target_link_libraries(fractal-julia PRIVATE 
    fmt::fmt
    SFML::Graphics SFML::Window SFML::Audio SFML::System
    ${MPI_LIB}/impi.lib
   )
