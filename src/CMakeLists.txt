add_library(
    TriangleApplication
    TriangleApplication.hpp
    TriangleApplication.cpp
    )
add_library(
    Global
    global.hpp
    global.cpp
    )

find_package(Vulkan REQUIRED)
find_package(PkgConfig REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})
pkg_search_module(GLFW REQUIRED glfw3)

target_include_directories(TriangleApplication PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Global PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
    Global
    ${GLFW_STATIC_LIBRARIES}
    Vulkan::Vulkan
    )
target_link_libraries(
    TriangleApplication
    Global
    )
