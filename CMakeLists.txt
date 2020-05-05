cmake_minimum_required(VERSION 3.17)
project(vulkanTest)

find_package(Vulkan REQUIRED)
find_package(PkgConfig REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})
add_executable(vulkanTest main.cpp)

target_link_libraries(
    vulkanTest
    ${GLFW_STATIC_LIBRARIES}
    Vulkan::Vulkan
    )
