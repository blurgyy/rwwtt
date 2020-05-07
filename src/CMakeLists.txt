add_library(
    TriangleApplication
    TriangleApplication.hpp
    TriangleApplication.cpp
    )

find_package(Vulkan REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

target_include_directories(TriangleApplication PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
    TriangleApplication
    ${GLFW_STATIC_LIBRARIES}
    Vulkan::Vulkan
    )
