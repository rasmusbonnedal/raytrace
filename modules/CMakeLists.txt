# Mapbox variant library
add_library(variant INTERFACE)
target_include_directories(variant INTERFACE variant/include)

# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")
add_subdirectory(glfw)
target_include_directories(glfw PUBLIC glfw/include)
if (APPLE)
   find_library(COCOA_LIBRARY Cocoa)
   find_library(IOKIT_LIBRARY IOKit)
   target_link_libraries(glfw INTERFACE ${COCOA_LIBRARY} ${IOKIT_LIBRARY})
endif ()

# Dear ImGui
add_library(imgui_glfw
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/examples/libs/gl3w/GL/gl3w.c
    imgui/examples/opengl3_example/imgui_impl_glfw_gl3.cpp
)
target_include_directories(imgui_glfw PUBLIC imgui imgui/examples/opengl3_example imgui/examples/libs/gl3w)
target_link_libraries(imgui_glfw glfw)