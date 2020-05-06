cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 17)

project(vulkanTest)
add_executable(vulkanTest main.cpp)

add_subdirectory(src)

add_library( amdvlk SHARED IMPORTED GLOBAL )
set_target_properties(amdvlk PROPERTIES IMPORTED_LOCATION "/usr/lib/x86_64-linux-gnu/amdvlk64.so")

target_link_libraries(
    vulkanTest
    TriangleApplication
    # amdvlk
    )
