cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

project(vulkanTest)
add_executable(vulkanTest main.cpp)

add_subdirectory(src)

target_link_libraries(
    vulkanTest
    TriangleApplication
    )
