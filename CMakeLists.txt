cmake_minimum_required(VERSION 3.17)

project(vulkanTest)
add_executable(vulkanTest main.cpp)

add_subdirectory(src)

target_link_libraries(
    vulkanTest
    PRIVATE TriangleApplication
    )
